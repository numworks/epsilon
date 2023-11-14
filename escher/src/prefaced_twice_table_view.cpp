#include <escher/prefaced_twice_table_view.h>

namespace Escher {

PrefacedTwiceTableView::PrefacedTwiceTableView(
    int prefaceRow, int prefaceColumn, Responder* parentResponder,
    SelectableTableView* mainTableView, TableViewDataSource* cellsDataSource,
    SelectableTableViewDelegate* delegate,
    PrefacedTableViewDelegate* prefacedTableViewDelegate)
    : PrefacedTableView(prefaceRow, parentResponder, mainTableView,
                        cellsDataSource, delegate, prefacedTableViewDelegate),
      m_columnPrefaceDataSource(prefaceColumn, cellsDataSource),
      m_columnPrefaceView(&m_columnPrefaceDataSource,
                          &m_columnPrefaceDataSource),
      m_prefaceIntersectionDataSource(prefaceRow, &m_columnPrefaceDataSource),
      m_prefaceIntersectionView(&m_prefaceIntersectionDataSource,
                                &m_prefaceIntersectionDataSource),
      m_mainTableViewLeftMargin(0) {
  m_columnPrefaceView.hideScrollBars();
}

void PrefacedTwiceTableView::setMargins(KDMargins m) {
  // Main table and row preface
  PrefacedTableView::setMargins(m);
  m_mainTableViewLeftMargin = m.left();
  // Column preface
  m_columnPrefaceView.margins()->setTop(0);
  m_columnPrefaceView.margins()->setBottom(m.bottom());
  m_columnPrefaceView.margins()->setHorizontal({0, 0});
}

void PrefacedTwiceTableView::setBackgroundColor(KDColor color) {
  // Main table and row preface
  PrefacedTableView::setBackgroundColor(color);
  // Column preface
  m_columnPrefaceView.setBackgroundColor(color);
  // Intersection preface
  m_prefaceIntersectionView.setBackgroundColor(color);
}

void PrefacedTwiceTableView::setCellOverlap(KDCoordinate horizontal,
                                            KDCoordinate vertical) {
  // Main table and row preface
  PrefacedTableView::setCellOverlap(horizontal, vertical);
  // Column preface
  m_columnPrefaceView.setHorizontalCellOverlap(horizontal);
  m_columnPrefaceView.setVerticalCellOverlap(vertical);
  // Intersection preface
  m_prefaceIntersectionView.setHorizontalCellOverlap(horizontal);
  m_prefaceIntersectionView.setVerticalCellOverlap(vertical);
}

void PrefacedTwiceTableView::resetSizeAndOffsetMemoization() {
  // Main table and row preface
  PrefacedTableView::resetSizeAndOffsetMemoization();
  // Column preface
  m_columnPrefaceView.resetSizeAndOffsetMemoization();
  // Intersection preface
  m_prefaceIntersectionView.resetSizeAndOffsetMemoization();
}

View* PrefacedTwiceTableView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return m_mainTableView;
    case 1:
      return &m_rowPrefaceView;
    case 2:
      return &m_columnPrefaceView;
    case 3:
      return &m_prefaceIntersectionView;
    case 4:
      return m_barDecorator.verticalBar();
    default:
      assert(index == 5);
      return m_barDecorator.horizontalBar();
  }
}

void PrefacedTwiceTableView::resetContentOffset() {
  // Main table and row preface
  PrefacedTableView::resetContentOffset();
  // Column preface
  m_columnPrefaceView.resetScroll();
  // Intersection preface
  m_prefaceIntersectionView.resetScroll();
}

void PrefacedTwiceTableView::layoutSubviewsInRect(KDRect rect, bool force) {
  assert(rect == bounds());
  if (m_prefacedDelegate) {
    m_columnPrefaceDataSource.setPrefaceColumn(
        m_prefacedDelegate->columnToFreeze());
  }
  bool hideColumnPreface =
      m_mainTableView->selectedRow() == -1 ||
      (m_mainTableView->invisibleWidth() <=
       m_columnPrefaceDataSource.cumulatedWidthAtPrefaceColumn(false));
  if (hideColumnPreface) {
    // Main table and row preface
    m_mainTableView->margins()->setLeft(m_mainTableViewLeftMargin);
    PrefacedTableView::layoutSubviewsInRect(bounds(), force);

    // Column preface
    setChildFrame(&m_columnPrefaceView, KDRectZero, force);

    // Intersection preface
    setChildFrame(&m_prefaceIntersectionView, KDRectZero, force);
  } else {
    KDCoordinate columnPrefaceWidth =
        m_columnPrefaceView.minimalSizeForOptimalDisplay().width() +
        m_columnPrefaceDataSource.separatorAfterPrefaceColumn();

    // Main table and row preface
    m_mainTableView->margins()->setLeft(0);
    PrefacedTableView::layoutSubviewsInRect(
        KDRect(columnPrefaceWidth, 0, bounds().width() - columnPrefaceWidth,
               bounds().height()),
        force);

    // Column preface
    KDCoordinate rowPrefaceHeight = m_rowPrefaceView.bounds().height();
    m_columnPrefaceView.margins()->setTop(m_mainTableView->margins()->top());
    m_columnPrefaceView.setContentOffset(
        KDPoint(0, m_mainTableView->contentOffset().y()));
    setChildFrame(&m_columnPrefaceView,
                  KDRect(0, rowPrefaceHeight, columnPrefaceWidth,
                         bounds().height() - rowPrefaceHeight),
                  force);
    assert(m_columnPrefaceView.margins()->left() == 0);
    assert(m_columnPrefaceView.margins()->vertical() ==
           m_mainTableView->margins()->vertical());

    // Intersection preface
    m_prefaceIntersectionView.margins()->setHorizontal(
        m_columnPrefaceView.margins()->horizontal());
    m_prefaceIntersectionView.margins()->setRight(
        m_columnPrefaceView.margins()->right());
    m_prefaceIntersectionView.margins()->setBottom(
        m_rowPrefaceView.margins()->bottom());
    setChildFrame(&m_prefaceIntersectionView,
                  KDRect(0, 0, rowPrefaceHeight ? columnPrefaceWidth : 0,
                         rowPrefaceHeight),
                  force);
    assert(m_prefaceIntersectionView.margins()->horizontal() ==
           m_columnPrefaceView.margins()->horizontal());
    assert(m_prefaceIntersectionView.margins()->vertical() ==
           m_rowPrefaceView.margins()->vertical());
    assert(rowPrefaceHeight == 0 ||
           m_prefaceIntersectionView.bounds().size() ==
               KDSize(columnPrefaceWidth, rowPrefaceHeight));
  }
}

KDCoordinate
PrefacedTwiceTableView::horizontalScrollToAddToHidePrefacesInMainTable(
    bool hideColumnPreface) const {
  return hideColumnPreface
             ? 0
             : std::max(m_columnPrefaceDataSource.cumulatedWidthAtPrefaceColumn(
                            true) -
                            m_mainTableView->invisibleWidth(),
                        0);
}

KDCoordinate
PrefacedTwiceTableView::ColumnPrefaceDataSource::cumulatedWidthAtPrefaceColumn(
    bool after) const {
  // Do not alter main dataSource memoization
  assert(m_prefaceColumn >= 0);
  int column = m_prefaceColumn + after;
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result = m_mainDataSource->cumulatedWidthBeforeColumn(column) +
                        m_mainDataSource->separatorBeforeColumn(column);
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

HighlightCell* PrefacedTwiceTableView::ColumnPrefaceDataSource::reusableCell(
    int index, int type) {
  /* The prefaced view and the main view must have different reusable cells to
   * avoid conflicts when layouting. To avoid creating a whole set of reusable
   * cells for the prefaced view, we use a hack : there is enough reusable cells
   * for "prefacedView + cropped mainView" in mainView, indeed we juste take the
   * last ones for the prefacedView (mainView will takes the first ones).
   * WARNING : this will works only because row preface uses the first ones (see
   * comment in IntermediaryDataSource::reusableCell), this way there will not
   * be conflicts between the two preface view. */
  return m_mainDataSource->reusableCell(
      m_mainDataSource->reusableCellCount(type) - 1 - index, type);
}

KDCoordinate PrefacedTwiceTableView::ColumnPrefaceDataSource::
    nonMemoizedCumulatedWidthBeforeColumn(int column) {
  // Do not alter main dataSource memoization
  assert(column == 0 || column == 1);
  assert(m_prefaceColumn >= 0);
  m_mainDataSource->lockSizeMemoization(true);
  KDCoordinate result =
      column == 1 ? m_mainDataSource->columnWidth(m_prefaceColumn, false) : 0;
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

int PrefacedTwiceTableView::ColumnPrefaceDataSource::
    nonMemoizedColumnAfterCumulatedWidth(KDCoordinate offsetX) {
  // Do not alter main dataSource memoization
  assert(m_prefaceColumn >= 0);
  m_mainDataSource->lockSizeMemoization(true);
  int result =
      offsetX < m_mainDataSource->columnWidth(m_prefaceColumn, false) ? 0 : 1;
  m_mainDataSource->lockSizeMemoization(false);
  return result;
}

}  // namespace Escher
