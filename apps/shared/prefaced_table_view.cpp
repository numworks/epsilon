#include "prefaced_table_view.h"

using namespace Escher;

namespace Shared {

PrefacedTableView::PrefacedTableView(
    int prefaceRow, Responder* parentResponder,
    SelectableTableView* mainTableView, TableViewDataSource* cellsDataSource,
    SelectableTableViewDelegate* delegate,
    PrefacedTableViewDelegate* prefacedTableViewDelegate)
    : Escher::Responder(parentResponder),
      m_rowPrefaceDataSource(prefaceRow, cellsDataSource),
      m_rowPrefaceView(&m_rowPrefaceDataSource, &m_rowPrefaceDataSource),
      m_mainTableView(mainTableView),
      m_marginDelegate(nullptr),
      m_prefacedDelegate(prefacedTableViewDelegate),
      m_mainTableDelegate(delegate),
      m_mainTableViewTopMargin(0) {
  m_mainTableView->setParentResponder(parentResponder);
  m_mainTableView->setDelegate(this);
  m_rowPrefaceView.setDecoratorType(ScrollView::Decorator::Type::None);
}

void PrefacedTableView::setMargins(KDCoordinate top, KDCoordinate right,
                                   KDCoordinate bottom, KDCoordinate left) {
  // Main table
  m_mainTableView->setTopMargin(top);
  m_mainTableView->setRightMargin(right);
  m_mainTableView->setBottomMargin(bottom);
  m_mainTableView->setLeftMargin(left);
  m_mainTableViewTopMargin = top;
  // Row preface
  m_rowPrefaceView.setLeftMargin(left);
  m_rowPrefaceView.setRightMargin(right);
  m_rowPrefaceView.setTopMargin(0);
  m_rowPrefaceView.setBottomMargin(0);
}

void PrefacedTableView::setBackgroundColor(KDColor color) {
  // Main table
  m_mainTableView->setBackgroundColor(color);
  // Row preface
  m_rowPrefaceView.setBackgroundColor(color);
}

void PrefacedTableView::setCellOverlap(KDCoordinate horizontal,
                                       KDCoordinate vertical) {
  // Main table
  m_mainTableView->setHorizontalCellOverlap(horizontal);
  m_mainTableView->setVerticalCellOverlap(vertical);
  // Row preface
  m_rowPrefaceView.setHorizontalCellOverlap(horizontal);
  m_rowPrefaceView.setVerticalCellOverlap(vertical);
}

void PrefacedTableView::tableViewDidChangeSelection(
    Escher::SelectableTableView* t, int previousSelectedCol,
    int previousSelectedRow, bool withinTemporarySelection) {
  assert(t == m_mainTableView);
  if (withinTemporarySelection) {
    return;
  }
  if (m_mainTableDelegate) {
    m_mainTableDelegate->tableViewDidChangeSelection(
        t, previousSelectedCol, previousSelectedRow, withinTemporarySelection);
  }
}

void PrefacedTableView::resetContentOffset() {
  // Main table
  m_mainTableView->setContentOffset(KDPointZero);
  // Row preface
  m_rowPrefaceView.setContentOffset(KDPointZero);
}

void PrefacedTableView::tableViewDidChangeSelectionAndDidScroll(
    Escher::SelectableTableView* t, int previousSelectedCol,
    int previousSelectedRow, bool withinTemporarySelection) {
  assert(t == m_mainTableView);
  if (withinTemporarySelection) {
    return;
  }
  if (m_mainTableDelegate) {
    m_mainTableDelegate->tableViewDidChangeSelectionAndDidScroll(
        t, previousSelectedCol, previousSelectedRow, withinTemporarySelection);
  }
  if (m_mainTableView->selectedRow() == -1) {
    resetContentOffset();
  }
  layoutSubviews();
}

void PrefacedTableView::layoutSubviewsInRect(KDRect rect, bool force) {
  KDCoordinate rowPrefaceHeight =
      m_rowPrefaceView.minimalSizeForOptimalDisplay().height();
  bool rowPrefaceIsTooLarge =
      m_prefacedDelegate &&
      rowPrefaceHeight > m_prefacedDelegate->maxRowPrefaceHeight();
  bool hideRowPreface =
      rowPrefaceIsTooLarge ||
      m_rowPrefaceDataSource.prefaceIsAfterOffset(
          m_mainTableView->contentOffset().y(), m_mainTableView->topMargin()) ||
      m_mainTableView->selectedRow() == -1;
  ScrollViewVerticalBar* verticalBar =
      static_cast<TableView::BarDecorator*>(m_mainTableView->decorator())
          ->verticalBar();

  // Main table
  if (hideRowPreface) {
    m_mainTableView->setTopMargin(
        m_mainTableViewTopMargin);  // WARNING: If preface row can change, top
                                    // margin is dependant on row frozen (follow
                                    // the implementation used for column
                                    // preface)
    m_mainTableView->setFrame(rect, force);
    verticalBar->setTopMargin(rowPrefaceHeight + 2 * m_mainTableViewTopMargin);
  } else {
    // WARNING: If we need a separator below the preface row, we should set a
    // bottom margin for rowPrefaceView here (follow the implementation used for
    // column preface)
    m_mainTableView->setTopMargin(0);
    m_mainTableView->setFrame(
        KDRect(rect.x(), rect.y() + rowPrefaceHeight, rect.width(),
               rect.height() - rowPrefaceHeight),
        force);
    verticalBar->setTopMargin(2 * m_mainTableViewTopMargin);
  }

  if (m_mainTableView->selectedRow() >= 0) {
    /* Scroll to update the content offset with the new frame. */
    m_mainTableView->scrollToCell(m_mainTableView->selectedColumn(),
                                  m_mainTableView->selectedRow());
  }

  // Row preface
  if (hideRowPreface) {
    m_rowPrefaceView.setFrame(KDRectZero, force);
  } else {
    m_rowPrefaceView.setLeftMargin(m_mainTableView->leftMargin());
    m_rowPrefaceView.setContentOffset(
        KDPoint(m_mainTableView->contentOffset().x(), 0));
    m_rowPrefaceView.setFrame(
        KDRect(rect.x(), rect.y(), rect.width(), rowPrefaceHeight), force);
    assert(m_rowPrefaceView.leftMargin() == m_mainTableView->leftMargin());
    assert(m_rowPrefaceView.rightMargin() == m_mainTableView->rightMargin());
    assert(m_rowPrefaceView.topMargin() == 0);
  }
}

void PrefacedTableView::layoutSubviews(bool force) {
  layoutSubviewsInRect(bounds(), force);
}

HighlightCell* PrefacedTableView::IntermediaryDataSource::reusableCell(
    int index, int type) {
  /* The prefaced view and the main view must have different reusable cells to
   * avoid conflicts when layouting. In most cases, the preface needs cell types
   * that will no longer be needed by the main datasource (for example title
   * types). Thus we can directly use the reusable cells of main datasource.
   * WARNING: if the preface needs the same types of cells than main datasource
   * or another preface, this method should be overriden either:
   *   - starting from the end of main datasource reusable cells if it doesn't
   * create conflicts
   *   - creating new reusable cells for the preface */
  return m_mainDataSource->reusableCell(index, type);
}

KDCoordinate PrefacedTableView::IntermediaryDataSource::nonMemoizedColumnWidth(
    int i) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  KDCoordinate result =
      m_mainDataSource->columnWidth(columnIndexInMainDataSource(i));
  m_mainDataSource->lockMemoization(false);
  return result;
}

KDCoordinate PrefacedTableView::IntermediaryDataSource::nonMemoizedRowHeight(
    int j) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  KDCoordinate result =
      m_mainDataSource->rowHeight(rowIndexInMainDataSource(j));
  m_mainDataSource->lockMemoization(false);
  return result;
}

KDCoordinate
PrefacedTableView::IntermediaryDataSource::nonMemoizedCumulatedWidthBeforeIndex(
    int i) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  KDCoordinate result = m_mainDataSource->cumulatedWidthBeforeIndex(
      columnIndexInMainDataSource(i));
  m_mainDataSource->lockMemoization(false);
  return result;
}

KDCoordinate PrefacedTableView::IntermediaryDataSource::
    nonMemoizedCumulatedHeightBeforeIndex(int j) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  KDCoordinate result =
      m_mainDataSource->cumulatedHeightBeforeIndex(rowIndexInMainDataSource(j));
  m_mainDataSource->lockMemoization(false);
  return result;
}

int PrefacedTableView::IntermediaryDataSource::
    nonMemoizedIndexAfterCumulatedWidth(KDCoordinate offsetX) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  int result = m_mainDataSource->indexAfterCumulatedWidth(offsetX);
  m_mainDataSource->lockMemoization(false);
  return result;
}

int PrefacedTableView::IntermediaryDataSource::
    nonMemoizedIndexAfterCumulatedHeight(KDCoordinate offsetY) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  int result = m_mainDataSource->indexAfterCumulatedHeight(offsetY);
  m_mainDataSource->lockMemoization(false);
  return result;
}

bool PrefacedTableView::RowPrefaceDataSource::prefaceIsAfterOffset(
    KDCoordinate offsetY, KDCoordinate topMargin) const {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  // y offset includes top margin
  bool result = offsetY - topMargin <=
                m_mainDataSource->cumulatedHeightBeforeIndex(m_prefaceRow);
  m_mainDataSource->lockMemoization(false);
  return result;
}

KDCoordinate
PrefacedTableView::RowPrefaceDataSource::nonMemoizedCumulatedHeightBeforeIndex(
    int j) {
  // Do not alter main dataSource memoization
  assert(j == 0 || j == 1);
  m_mainDataSource->lockMemoization(true);
  KDCoordinate result = j == 1 ? m_mainDataSource->rowHeight(m_prefaceRow) : 0;
  m_mainDataSource->lockMemoization(false);
  return result;
}

int PrefacedTableView::RowPrefaceDataSource::
    nonMemoizedIndexAfterCumulatedHeight(KDCoordinate offsetY) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  int result = offsetY < m_mainDataSource->rowHeight(m_prefaceRow) ? 0 : 1;
  m_mainDataSource->lockMemoization(false);
  return result;
}

}  // namespace Shared
