#include "prefaced_twice_table_view.h"

using namespace Escher;

namespace Shared {

PrefacedTwiceTableView::PrefacedTwiceTableView(int prefaceRow, int prefaceColumn, Escher::Responder * parentResponder, Escher::SelectableTableView * mainTableView, Escher::TableViewDataSource * cellsDataSource, Escher::SelectableTableViewDelegate * delegate, PrefacedTableViewDelegate * prefacedTableViewDelegate) :
  PrefacedTableView(prefaceRow, parentResponder, mainTableView, cellsDataSource, delegate, prefacedTableViewDelegate),
  m_columnPrefaceDataSource(prefaceColumn, cellsDataSource),
  m_columnPrefaceView(&m_columnPrefaceDataSource, &m_columnPrefaceDataSource),
  m_prefaceIntersectionDataSource(prefaceRow, &m_columnPrefaceDataSource),
  m_prefaceIntersectionView(&m_prefaceIntersectionDataSource, &m_prefaceIntersectionDataSource),
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
  m_prefaceIntersectionView.setBackgroundColor(color);
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
  case 2:
    return &m_columnPrefaceView;
  default:
    assert(index == 3);
    return &m_prefaceIntersectionView;
  }
}

void PrefacedTwiceTableView::layoutSubviews(bool force) {
  bool loop = true;
  while (loop) {
    privateLayoutSubviews(force);
    loop = m_prefacedDelegate && m_columnPrefaceDataSource.setPrefaceColumn(m_prefacedDelegate->columnToFreeze());
  }
}

void PrefacedTwiceTableView::privateLayoutSubviews(bool force) {
  bool hideColumnPreface = m_mainTableView->selectedRow() == -1 || m_columnPrefaceDataSource.prefaceIsAfterOffset(m_mainTableView->contentOffset().x(), m_mainTableView->leftMargin());
  if (hideColumnPreface) {
    // Main table and row preface
    m_mainTableView->setLeftMargin(m_prefacedDelegate && m_columnPrefaceDataSource.prefaceColumn() != m_prefacedDelegate->firstFeezableColumn() ? 0 : m_mainTableViewLeftMargin);
    m_rowPrefaceView.setLeftMargin(m_mainTableView->leftMargin());
    layoutSubviewsInRect(bounds(), force);

    // Column preface and intersection preface
    m_columnPrefaceView.setFrame(KDRectZero, force);
    m_prefaceIntersectionView.setFrame(KDRectZero, force);
  } else {
    m_columnPrefaceView.setRightMargin(m_marginDelegate ? m_marginDelegate->columnPrefaceRightMargin() : 0);
    KDCoordinate columnPrefaceWidth = m_columnPrefaceView.minimalSizeForOptimalDisplay().width();

    // Main table and row preface
    m_mainTableView->setLeftMargin(0);
    m_rowPrefaceView.setLeftMargin(m_mainTableView->leftMargin());
    layoutSubviewsInRect(KDRect(columnPrefaceWidth, 0, bounds().width() - columnPrefaceWidth, bounds().height()), force);

    // Column preface and intersection preface
    KDCoordinate rowPrefaceHeight = m_rowPrefaceView.bounds().height();
    m_columnPrefaceView.setTopMargin(m_mainTableView->topMargin());
    m_columnPrefaceView.setContentOffset(KDPoint(0, m_mainTableView->contentOffset().y()));
    m_columnPrefaceView.setFrame(KDRect(0, rowPrefaceHeight, columnPrefaceWidth, bounds().height() - rowPrefaceHeight), force);
    m_prefaceIntersectionView.setFrame(KDRect(0, 0, rowPrefaceHeight ? columnPrefaceWidth : 0, rowPrefaceHeight), force);
  }
}

bool PrefacedTwiceTableView::ColumnPrefaceDataSource::setPrefaceColumn(int column) {
  if (m_prefaceColumn != column) {
    m_prefaceColumn = column;
    return true;
  }
  return false;
}

bool PrefacedTwiceTableView::ColumnPrefaceDataSource::prefaceIsAfterOffset(KDCoordinate offsetX, KDCoordinate leftMargin) const {
  // Do not alter main dataSource memoization
  m_mainDataSource->lockMemoization(true);
  // x offset includes left margin
  bool result = offsetX - leftMargin <= m_mainDataSource->cumulatedWidthBeforeIndex(m_prefaceColumn);
  m_mainDataSource->lockMemoization(false);
  return result;
}

HighlightCell * PrefacedTwiceTableView::ColumnPrefaceDataSource::reusableCell(int index, int type) {
  /* The prefaced view and the main view must have different reusable cells to avoid conflicts
   * when layouting. To avoid creating a whole set of reusable cells for the prefaced view,
   * we use a hack : there is enough reusable cells for "prefacedView + cropped mainView" in
   * mainView, indeed we juste take the last ones for the prefacedView (mainView will takes
   * the first ones).
   * WARNING : this will works only because row preface uses the first ones (see comment in
   * IntermediaryDataSource::reusableCell), this way there will not be conflicts between the
   * two preface view. */
  return m_mainDataSource->reusableCell(m_mainDataSource->reusableCellCount(type) - 1 - index, type);
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
