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
  // Main table and row preface
  PrefacedTableView::setMargins(top, right, bottom, left);
  m_mainTableViewLeftMargin = left;
  // Column preface
  m_columnPrefaceView.setTopMargin(top);
  m_columnPrefaceView.setBottomMargin(bottom);
  m_columnPrefaceView.setLeftMargin(0);
  // Intersection preface
  m_columnPrefaceView.setTopMargin(0);
  m_columnPrefaceView.setLeftMargin(0);
}

void PrefacedTwiceTableView::setBackgroundColor(KDColor color) {
  // Main table and row preface
  PrefacedTableView::setBackgroundColor(color);
  // Column preface
  m_columnPrefaceView.setBackgroundColor(color);
  // Intersection preface
  m_prefaceIntersectionView.setBackgroundColor(color);
}

void PrefacedTwiceTableView::setCellOverlap(KDCoordinate horizontal, KDCoordinate vertical) {
  // Main table and row preface
  PrefacedTableView::setCellOverlap(horizontal, vertical);
  // Column preface
  m_columnPrefaceView.setHorizontalCellOverlap(horizontal);
  m_columnPrefaceView.setVerticalCellOverlap(vertical);
  // Intersection preface
  m_prefaceIntersectionView.setHorizontalCellOverlap(horizontal);
  m_prefaceIntersectionView.setVerticalCellOverlap(vertical);
}

void PrefacedTwiceTableView::resetDataSourceSizeMemoization() {
  // Main table and row preface
  PrefacedTableView::resetDataSourceSizeMemoization();
  // Column preface
  m_columnPrefaceDataSource.resetMemoization();
  // Intersection preface
  m_prefaceIntersectionDataSource.resetMemoization();
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

void PrefacedTwiceTableView::resetContentOffset() {
  // Main table and row preface
  PrefacedTableView::resetContentOffset();
  // Column preface
  m_columnPrefaceView.setContentOffset(KDPointZero);
  // Intersection preface
  m_prefaceIntersectionView.setContentOffset(KDPointZero);
}

void PrefacedTwiceTableView::layoutSubviews(bool force) {
  if (m_prefacedDelegate) {
    m_columnPrefaceDataSource.setPrefaceColumn(m_prefacedDelegate->columnToFreeze());
  }
  bool hideColumnPreface = m_mainTableView->selectedRow() == -1 || m_columnPrefaceDataSource.prefaceColumn() == -1 || m_columnPrefaceDataSource.prefaceIsAfterOffset(m_mainTableView->contentOffset().x(), m_mainTableView->leftMargin());
  if (hideColumnPreface) {
    // Main table and row preface
    const KDCoordinate leftMargin = m_prefacedDelegate && m_columnPrefaceDataSource.prefaceColumn() > m_prefacedDelegate->firstFeezableColumn() ? 0 : m_mainTableViewLeftMargin;
    m_mainTableView->setLeftMargin(leftMargin);
    m_rowPrefaceView.setLeftMargin(leftMargin);
    layoutSubviewsInRect(bounds(), force);

    // Column preface
    m_columnPrefaceView.setFrame(KDRectZero, force);

    // Intersection preface
    m_prefaceIntersectionView.setFrame(KDRectZero, force);
  } else {
    m_columnPrefaceView.setRightMargin(m_marginDelegate ? m_marginDelegate->columnPrefaceRightMargin() : 0);
    KDCoordinate columnPrefaceWidth = m_columnPrefaceView.minimalSizeForOptimalDisplay().width();

    // Main table and row preface
    const KDCoordinate leftMargin = 0;
    m_mainTableView->setLeftMargin(leftMargin);
    m_rowPrefaceView.setLeftMargin(leftMargin);
    layoutSubviewsInRect(KDRect(columnPrefaceWidth, 0, bounds().width() - columnPrefaceWidth, bounds().height()), force);

    // Column preface
    KDCoordinate rowPrefaceHeight = m_rowPrefaceView.bounds().height();
    m_columnPrefaceView.setTopMargin(m_mainTableView->topMargin());
    m_columnPrefaceView.setContentOffset(KDPoint(0, m_mainTableView->contentOffset().y()));
    m_columnPrefaceView.setFrame(KDRect(0, rowPrefaceHeight, columnPrefaceWidth, bounds().height() - rowPrefaceHeight), force);
    assert(m_columnPrefaceView.leftMargin() == 0);
    assert(m_columnPrefaceView.topMargin() == m_mainTableView->topMargin());
    assert(m_columnPrefaceView.bottomMargin() == m_mainTableView->bottomMargin());

    // Intersection preface
    m_prefaceIntersectionView.setRightMargin(m_columnPrefaceView.rightMargin());
    m_prefaceIntersectionView.setBottomMargin(m_rowPrefaceView.bottomMargin());
    m_prefaceIntersectionView.setFrame(KDRect(0, 0, rowPrefaceHeight ? columnPrefaceWidth : 0, rowPrefaceHeight), force);
    assert(m_prefaceIntersectionView.leftMargin() == m_columnPrefaceView.leftMargin());
    assert(m_prefaceIntersectionView.rightMargin() == m_columnPrefaceView.rightMargin());
    assert(m_prefaceIntersectionView.topMargin() == m_rowPrefaceView.topMargin());
    assert(m_prefaceIntersectionView.bottomMargin() == m_rowPrefaceView.bottomMargin());
    assert(rowPrefaceHeight == 0 || m_prefaceIntersectionView.minimalSizeForOptimalDisplay() == KDSize(columnPrefaceWidth, rowPrefaceHeight));
  }
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
