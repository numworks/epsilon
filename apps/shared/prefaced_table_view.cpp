#include "prefaced_table_view.h"

using namespace Escher;

namespace Shared {

  PrefacedTableView::PrefacedTableView(int prefaceRow, Responder * parentResponder, SelectableTableView * mainTableView, TableViewDataSource * cellsDataSource, SelectableTableViewDelegate * delegate, PrefacedTableViewDelegate * prefacedTableViewDelegate) :
  Escher::Responder(parentResponder),
  m_rowPrefaceDataSource(prefaceRow, cellsDataSource),
  m_rowPrefaceView(&m_rowPrefaceDataSource, &m_rowPrefaceDataSource),
  m_mainTableView(mainTableView),
  m_marginDelegate(nullptr),
  m_mainTableDelegate(delegate),
  m_prefacedDelegate(prefacedTableViewDelegate),
  m_mainTableViewTopMargin(0)
{
  m_mainTableView->setParentResponder(parentResponder);
  m_mainTableView->setDelegate(this);
  m_rowPrefaceView.setDecoratorType(ScrollView::Decorator::Type::None);
}

void PrefacedTableView::setMargins(KDCoordinate top, KDCoordinate right, KDCoordinate bottom, KDCoordinate left) {
  m_mainTableView->setTopMargin(top);
  m_mainTableView->setRightMargin(right);
  m_mainTableView->setBottomMargin(bottom);
  m_mainTableView->setLeftMargin(left);
  m_mainTableViewTopMargin = top;

  m_rowPrefaceView.setLeftMargin(left);
  m_rowPrefaceView.setRightMargin(right);
}

void PrefacedTableView::setBackgroundColor(KDColor color) {
  m_mainTableView->setBackgroundColor(color);
  m_rowPrefaceView.setBackgroundColor(color);
}

void PrefacedTableView::setCellOverlap(KDCoordinate horizontal, KDCoordinate vertical) {
  m_mainTableView->setHorizontalCellOverlap(horizontal);
  m_mainTableView->setVerticalCellOverlap(vertical);
  m_rowPrefaceView.setHorizontalCellOverlap(horizontal);
  m_rowPrefaceView.setVerticalCellOverlap(vertical);
}

void PrefacedTableView::tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  assert(t == m_mainTableView);
  if (m_mainTableDelegate) {
    m_mainTableDelegate->tableViewDidChangeSelection(t, previousSelectedCellX, previousSelectedCellY, withinTemporarySelection);
  }
}

void PrefacedTableView::tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  assert(t == m_mainTableView);
  if (m_mainTableDelegate) {
    m_mainTableDelegate->tableViewDidChangeSelectionAndDidScroll(t, previousSelectedCellX, previousSelectedCellY, withinTemporarySelection);
  }
  layoutSubviews();
}

void PrefacedTableView::layoutSubviewsInRect(KDRect rect, bool force) {
  KDCoordinate rowPrefaceHeight = m_rowPrefaceView.minimalSizeForOptimalDisplay().height();
  bool rowPrefaceIsTooLarge = m_prefacedDelegate && rowPrefaceHeight > m_prefacedDelegate->maxRowPrefaceHeight();
  bool hideRowPreface = rowPrefaceIsTooLarge || m_rowPrefaceDataSource.prefaceFullyInFrame(m_mainTableView->contentOffset().y()) || m_mainTableView->selectedRow() == -1;
  ScrollViewVerticalBar * verticalBar = static_cast<TableView::BarDecorator*>(m_mainTableView->decorator())->verticalBar();

  if (hideRowPreface) {
    m_mainTableView->setTopMargin(m_mainTableViewTopMargin);
    m_mainTableView->setFrame(rect, force);
    verticalBar->setTopMargin(rowPrefaceHeight + 2 * m_mainTableViewTopMargin);
  } else {
    m_mainTableView->setTopMargin(0);
    m_mainTableView->setFrame(KDRect(rect.x(), rect.y() + rowPrefaceHeight, rect.width(), rect.height() - rowPrefaceHeight), force);
    verticalBar->setTopMargin(2 * m_mainTableViewTopMargin);
  }

  if (m_mainTableView->selectedRow() >= 0) {
    /* Scroll to update the content offset with the new margins. */
    m_mainTableView->scrollToCell(m_mainTableView->selectedColumn(), m_mainTableView->selectedRow());
  }

  if (hideRowPreface) {
    m_rowPrefaceView.setFrame(KDRectZero, force);
  } else {
    m_rowPrefaceView.setContentOffset(KDPoint(m_mainTableView->contentOffset().x(), 0));
    m_rowPrefaceView.setFrame(KDRect(rect.x(), rect.y(), rect.width(), rowPrefaceHeight), force);
  }
}

void PrefacedTableView::layoutSubviews(bool force) {
  layoutSubviewsInRect(bounds(), force);
}

KDCoordinate PrefacedTableView::IntermediaryDataSource::nonMemoizedColumnWidth(int i) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  KDCoordinate result = m_mainDataSource->columnWidth(columnIndexInMainDataSource(i));
  m_mainDataSource->lockMemoization(false);
  return result;
}

KDCoordinate PrefacedTableView::IntermediaryDataSource::nonMemoizedRowHeight(int j) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  KDCoordinate result = m_mainDataSource->rowHeight(rowIndexInMainDataSource(j));
  m_mainDataSource->lockMemoization(false);
  return result;
}

KDCoordinate PrefacedTableView::IntermediaryDataSource::nonMemoizedCumulatedWidthBeforeIndex(int i) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  KDCoordinate result = m_mainDataSource->cumulatedWidthBeforeIndex(columnIndexInMainDataSource(i));
  m_mainDataSource->lockMemoization(false);
  return result;
}

KDCoordinate PrefacedTableView::IntermediaryDataSource::nonMemoizedCumulatedHeightBeforeIndex(int j) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  KDCoordinate result = m_mainDataSource->cumulatedHeightBeforeIndex(rowIndexInMainDataSource(j));
  m_mainDataSource->lockMemoization(false);
  return result;
}

int PrefacedTableView::IntermediaryDataSource::nonMemoizedIndexAfterCumulatedWidth(KDCoordinate offsetX) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  int result = m_mainDataSource->indexAfterCumulatedWidth(offsetX);
  m_mainDataSource->lockMemoization(false);
  return result;
}

int PrefacedTableView::IntermediaryDataSource::nonMemoizedIndexAfterCumulatedHeight(KDCoordinate offsetY) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  int result = m_mainDataSource->indexAfterCumulatedHeight(offsetY);
  m_mainDataSource->lockMemoization(false);
  return result;
}

bool PrefacedTableView::RowPrefaceDataSource::prefaceFullyInFrame(int offset) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  bool result = offset <= m_mainDataSource->cumulatedHeightBeforeIndex(m_prefaceRow);
  m_mainDataSource->lockMemoization(false);
  return result;
}

KDCoordinate PrefacedTableView::RowPrefaceDataSource::nonMemoizedCumulatedHeightBeforeIndex(int j) {
  // Do not alter main dataSource memoization
  assert(j == 0 || j == 1);
  m_mainDataSource->lockMemoization(true);
  KDCoordinate result = j == 1 ? m_mainDataSource->rowHeight(m_prefaceRow) : 0;
  m_mainDataSource->lockMemoization(false);
  return result;
}

int PrefacedTableView::RowPrefaceDataSource::nonMemoizedIndexAfterCumulatedHeight(KDCoordinate offsetY) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  int result = offsetY < m_mainDataSource->rowHeight(m_prefaceRow) ? 0 : 1;
  m_mainDataSource->lockMemoization(false);
  return result;
}

}
