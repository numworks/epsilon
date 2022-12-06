#include "prefaced_twice_table_view.h"

using namespace Escher;

namespace Shared {

PrefacedTwiceTableView::PrefacedTwiceTableView(int prefaceRow, int prefaceColumn, Escher::Responder * parentResponder, Escher::SelectableTableView * mainTableView, Escher::TableViewDataSource * cellsDataSource, Escher::SelectableTableViewDelegate * delegate, PrefacedTableViewDelegate * prefacedTableViewDelegate) :
  PrefacedTableView(prefaceRow, parentResponder, mainTableView, cellsDataSource, delegate, prefacedTableViewDelegate),
  m_columnPrefaceDataSource(prefaceColumn, cellsDataSource),
  m_columnPrefaceView(&m_columnPrefaceDataSource, &m_columnPrefaceDataSource),
  m_mainTableViewLeftMargin(0)
{
  m_columnPrefaceView.setDecoratorType(ScrollView::Decorator::Type::None);
}

void PrefacedTwiceTableView::setMargins(KDCoordinate top, KDCoordinate right, KDCoordinate bottom, KDCoordinate left) {
  m_mainTableViewLeftMargin = left;
  m_columnPrefaceView.setTopMargin(top);
  m_columnPrefaceView.setBottomMargin(bottom);
  PrefacedTableView::setMargins(top, right, bottom, left);
}

void PrefacedTwiceTableView::setBackgroundColor(KDColor color) {
  m_columnPrefaceView.setBackgroundColor(color);
  PrefacedTableView::setBackgroundColor(color);
}

void PrefacedTwiceTableView::setCellOverlap(KDCoordinate horizontal, KDCoordinate vertical) {
  m_columnPrefaceView.setHorizontalCellOverlap(horizontal);
  m_columnPrefaceView.setVerticalCellOverlap(vertical);
  PrefacedTableView::setCellOverlap(horizontal, vertical);
}

View * PrefacedTwiceTableView::subviewAtIndex(int index) {
  switch (index) {
  case 0:
    return m_mainTableView;
  case 1:
    return &m_rowPrefaceView;
  default:
    assert(index == 2);
    return &m_columnPrefaceView;
  }
}

void PrefacedTwiceTableView::layoutSubviews(bool force) {
  bool hideColumnPreface = m_mainTableView->selectedRow() == -1 || m_columnPrefaceDataSource.prefaceFullyInFrame(m_mainTableView->contentOffset().x());
  if (hideColumnPreface) {
    m_columnPrefaceView.setFrame(KDRectZero, force);
    m_mainTableView->setLeftMargin(m_mainTableViewLeftMargin);
    m_rowPrefaceView.setLeftMargin(m_mainTableViewLeftMargin);
    layoutSubviewsInRect(bounds(), force);
  } else {
    m_columnPrefaceView.setRightMargin(m_marginDelegate ? m_marginDelegate->columnPrefaceRightMargin(&m_columnPrefaceView) : 0);
    m_mainTableView->setLeftMargin(0);
    m_rowPrefaceView.setLeftMargin(0);
    KDCoordinate columnPrefaceWidth = m_columnPrefaceView.minimalSizeForOptimalDisplay().width();
    layoutSubviewsInRect(KDRect(columnPrefaceWidth, 0, bounds().width() - columnPrefaceWidth, bounds().height()), force);

    KDCoordinate rowPrefaceHeight = m_rowPrefaceView.bounds().height();
    m_columnPrefaceDataSource.setPrefaceRow(rowPrefaceHeight == 0 ? -1 : m_rowPrefaceDataSource.prefaceRow());
    m_columnPrefaceView.setContentOffset(KDPoint(0, m_mainTableView->contentOffset().y() - rowPrefaceHeight + m_columnPrefaceView.topMargin() - m_mainTableView->topMargin()));
    m_columnPrefaceView.setFrame(KDRect(0, 0, columnPrefaceWidth, bounds().height()), force);
  }
}

bool PrefacedTwiceTableView::ColumnPrefaceDataSource::prefaceFullyInFrame(int offset) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  bool result = offset <= m_mainDataSource->cumulatedWidthBeforeIndex(m_prefaceColumn);
  m_mainDataSource->lockMemoization(false);
  return result;
}

KDCoordinate PrefacedTwiceTableView::ColumnPrefaceDataSource::nonMemoizedCumulatedWidthBeforeIndex(int i) {
  // Do not alter main dataSource memoization
  assert(i == 0 || i == 1);
  m_mainDataSource->lockMemoization(true);
  KDCoordinate result = i == 1 ? m_mainDataSource->columnWidth(m_prefaceColumn) : 0;
  m_mainDataSource->lockMemoization(false);
  return result;
}

int PrefacedTwiceTableView::ColumnPrefaceDataSource::nonMemoizedIndexAfterCumulatedWidth(KDCoordinate offsetX) {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  int result = offsetX < m_mainDataSource->columnWidth(m_prefaceColumn) ? 0 : 1;
  m_mainDataSource->lockMemoization(false);
  return result;
}

}
