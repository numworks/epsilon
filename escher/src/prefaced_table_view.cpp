#include <escher/prefaced_table_view.h>

namespace Escher {

PrefacedTableView::PrefacedTableView(
    int prefaceRow, Responder* parentResponder,
    SelectableTableView* mainTableView, TableViewDataSource* cellsDataSource,
    SelectableTableViewDelegate* delegate,
    PrefacedTableViewDelegate* prefacedTableViewDelegate)
    : Responder(parentResponder),
      m_rowPrefaceDataSource(prefaceRow, cellsDataSource),
      m_rowPrefaceView(&m_rowPrefaceDataSource, &m_rowPrefaceDataSource),
      m_mainTableView(mainTableView),
      m_marginDelegate(nullptr),
      m_prefacedDelegate(prefacedTableViewDelegate),
      m_mainTableViewTopMargin(0),
      m_mainTableDelegate(delegate),
      m_actualOffsetWithMargins(KDPointZero) {
  m_mainTableView->setParentResponder(parentResponder);
  m_mainTableView->setDelegate(this);
  m_rowPrefaceView.hideScrollBars();
}

void PrefacedTableView::setMargins(KDMargins m) {
  // Main table
  m_mainTableView->setMargins(m);
  m_mainTableViewTopMargin = m.top();
  // Row preface
  m_rowPrefaceView.setMargins({m.horizontal(), KDVerticalMargins()});
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

void PrefacedTableView::resetContentOffset() {
  /* Since cells are shared between the table and its prefaces and since
   * TableView::layoutSubview first initSize on all cells before computing the
   * actual sizes, the preface resets could mess up the frames set by the main
   * one so call it first. */
  m_rowPrefaceView.resetScroll();
  m_mainTableView->resetScroll();
}

void PrefacedTableView::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
    KDPoint previousOffset, bool withinTemporarySelection) {
  assert(t == m_mainTableView);
  if (m_mainTableDelegate) {
    m_mainTableDelegate->tableViewDidChangeSelectionAndDidScroll(
        m_mainTableView, previousSelectedCol, previousSelectedRow,
        previousOffset, withinTemporarySelection);
  }
  if (m_mainTableView->selectedRow() == -1) {
    resetContentOffset();
  }
  layoutSubviews();

  // Update actual offset
  m_actualOffsetWithMargins =
      m_mainTableView->contentOffset()
          .relativeTo(relativeChildOrigin(m_mainTableView))
          .translatedBy(marginToAddForVirtualOffset());
  assert(m_actualOffsetWithMargins.x() >= 0 &&
         m_actualOffsetWithMargins.y() >= 0);
}

View* PrefacedTableView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return m_mainTableView;
    case 1:
      return &m_rowPrefaceView;
    case 2:
      return m_barDecorator.verticalBar();
    default:
      assert(index == 3);
      return m_barDecorator.horizontalBar();
  }
}

void PrefacedTableView::layoutSubviewsInRect(KDRect rect, bool force) {
  KDCoordinate rowPrefaceHeight =
      m_rowPrefaceView.minimalSizeForOptimalDisplay().height();
  bool rowPrefaceIsTooLarge =
      m_prefacedDelegate &&
      rowPrefaceHeight > m_prefacedDelegate->maxRowPrefaceHeight();
  bool hideRowPreface =
      m_mainTableView->selectedRow() == -1 || rowPrefaceIsTooLarge ||
      (m_mainTableView->contentOffset().y() -
           m_mainTableView->margins()->top() <=
       m_rowPrefaceDataSource.cumulatedHeightBeforePrefaceRow());

  // Main table
  if (hideRowPreface) {
    m_mainTableView->margins()->setTop(m_mainTableViewTopMargin);
    setChildFrame(m_mainTableView, rect, force);
  } else {
    /* WARNING: If we need a separator below the preface row, we should set a
     * bottom margin for rowPrefaceView here (follow the implementation used for
     * column preface) */
    m_mainTableView->margins()->setTop(0);
    setChildFrame(m_mainTableView,
                  KDRect(rect.x(), rect.y() + rowPrefaceHeight, rect.width(),
                         rect.height() - rowPrefaceHeight),
                  force);
  }

  if (m_mainTableView->selectedRow() >= 0) {
    /* Scroll to update the content offset with the new frame. */
    m_mainTableView->scrollToCell(m_mainTableView->selectedColumn(),
                                  m_mainTableView->selectedRow());
  }

  // Row preface
  if (hideRowPreface) {
    setChildFrame(&m_rowPrefaceView, KDRectZero, force);
  } else {
    m_rowPrefaceView.margins()->setLeft(m_mainTableView->margins()->left());
    m_rowPrefaceView.setContentOffset(
        KDPoint(m_mainTableView->contentOffset().x(), 0));
    setChildFrame(&m_rowPrefaceView,
                  KDRect(rect.x(), rect.y(), rect.width(), rowPrefaceHeight),
                  force);
    assert(m_rowPrefaceView.margins()->horizontal() ==
           m_mainTableView->margins()->horizontal());
    assert(m_rowPrefaceView.margins()->top() == 0);
  }
}

void PrefacedTableView::layoutScrollbars(bool force) {
  m_mainTableView->hideScrollBars();
  // Content offset if we had no prefaces hiding a part of the table
  KDPoint virtualOffset = m_mainTableView->contentOffset()
                              .relativeTo(relativeChildOrigin(m_mainTableView))
                              .translatedBy(marginToAddForVirtualOffset());
  KDRect r1 = KDRectZero;
  KDRect r2 = KDRectZero;
  m_barDecorator.layoutIndicators(
      this, m_mainTableView->minimalSizeForOptimalDisplay(), virtualOffset,
      bounds(), &r1, &r2, force);
}

void PrefacedTableView::layoutSubviews(bool force) {
  layoutSubviewsInRect(bounds(), force);
  layoutScrollbars(force);
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
    int column) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result =
      m_mainDataSource->columnWidth(columnInMainDataSource(column), false);
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

KDCoordinate PrefacedTableView::IntermediaryDataSource::nonMemoizedRowHeight(
    int row) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result =
      m_mainDataSource->rowHeight(rowInMainDataSource(row), false);
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

KDCoordinate PrefacedTableView::IntermediaryDataSource::
    nonMemoizedCumulatedWidthBeforeColumn(int column) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result = m_mainDataSource->cumulatedWidthBeforeColumn(
      columnInMainDataSource(column));
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

KDCoordinate
PrefacedTableView::IntermediaryDataSource::nonMemoizedCumulatedHeightBeforeRow(
    int row) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result =
      m_mainDataSource->cumulatedHeightBeforeRow(rowInMainDataSource(row));
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

int PrefacedTableView::IntermediaryDataSource::
    nonMemoizedColumnAfterCumulatedWidth(KDCoordinate offsetX) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  int result = m_mainDataSource->columnAfterCumulatedWidth(offsetX);
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

int PrefacedTableView::IntermediaryDataSource::
    nonMemoizedRowAfterCumulatedHeight(KDCoordinate offsetY) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  int result = m_mainDataSource->rowAfterCumulatedHeight(offsetY);
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

KDCoordinate
PrefacedTableView::RowPrefaceDataSource::cumulatedHeightBeforePrefaceRow()
    const {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result =
      m_mainDataSource->cumulatedHeightBeforeRow(m_prefaceRow) +
      m_mainDataSource->separatorBeforeRow(m_prefaceRow);
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

KDCoordinate
PrefacedTableView::RowPrefaceDataSource::nonMemoizedCumulatedHeightBeforeRow(
    int row) {
  // Do not alter main dataSource memoization
  assert(row == 0 || row == 1);
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result =
      row == 1 ? m_mainDataSource->rowHeight(m_prefaceRow, false) : 0;
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

int PrefacedTableView::RowPrefaceDataSource::nonMemoizedRowAfterCumulatedHeight(
    KDCoordinate offsetY) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockSizeMemoization(true);
  int result =
      offsetY < m_mainDataSource->rowHeight(m_prefaceRow, false) ? 0 : 1;
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

}  // namespace Escher
