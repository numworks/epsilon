#include <escher/table_view.h>
#include <escher/metric.h>

extern "C" {
#include <assert.h>
}

#define MIN(x,y) ((x)<(y) ? (x) : (y))

TableView::TableView(TableViewDataSource * dataSource, ScrollViewDataSource * scrollDataSource) :
  ScrollView(&m_contentView, scrollDataSource),
  m_contentView(this, dataSource, 0, 1)
{
}

KDSize TableView::minimalSizeForOptimalDisplay() const {
  return m_contentView.minimalSizeForOptimalDisplay();
}

TableViewDataSource * TableView::dataSource() {
  return m_contentView.dataSource();
}

// This method computes the minimal scrolling needed to properly display the
// requested cell.
void TableView::scrollToCell(int i, int j) {
  m_contentView.scrollToCell(i, j);
}

HighlightCell * TableView::cellAtLocation(int i, int j) {
  return m_contentView.cellAtLocation(i, j);
}

#if ESCHER_VIEW_LOGGING
const char * TableView::className() const {
  return "TableView";
}
#endif

void TableView::layoutSubviews() {
  // We only have to layout our contentView.
  // We will size it here, and ScrollView::layoutSubviews will position it.

  m_contentView.resizeToFitContent();

  ScrollView::layoutSubviews();
}

void TableView::reloadCellAtLocation(int i, int j) {
  m_contentView.reloadCellAtLocation(i, j);
}

/* TableView::ContentView */

TableView::ContentView::ContentView(TableView * tableView, TableViewDataSource * dataSource, KDCoordinate horizontalCellOverlap, KDCoordinate verticalCellOverlap) :
  View(),
  m_tableView(tableView),
  m_dataSource(dataSource),
  m_horizontalCellOverlap(horizontalCellOverlap),
  m_verticalCellOverlap(verticalCellOverlap)
{
}

KDSize TableView::ContentView::minimalSizeForOptimalDisplay() const {
  return KDSize(width(), height());
}

TableViewDataSource * TableView::ContentView::dataSource() {
  return m_dataSource;
}

KDCoordinate TableView::ContentView::columnWidth(int i) const {
  int columnWidth = m_dataSource->columnWidth(i);
  columnWidth = columnWidth ? columnWidth : m_tableView->maxContentWidthDisplayableWithoutScrolling();
  return columnWidth;
}

void TableView::ContentView::resizeToFitContent() {
  if (!(m_tableView->bounds() == KDRectZero)) {
    layoutSubviews();
    setSize(KDSize(width(), height()));
  }
}

KDCoordinate TableView::ContentView::height() const {
  return m_dataSource->cumulatedHeightFromIndex(m_dataSource->numberOfRows())+m_verticalCellOverlap;
}

KDCoordinate TableView::ContentView::width() const {
  int result = m_dataSource->cumulatedWidthFromIndex(m_dataSource->numberOfColumns())+m_horizontalCellOverlap;
  // handle the case of list: cumulatedWidthFromIndex() = 0
  return result ? result : m_tableView->maxContentWidthDisplayableWithoutScrolling();
}

void TableView::ContentView::scrollToCell(int x, int y) const {
  KDRect cellRect = KDRect(m_dataSource->cumulatedWidthFromIndex(x), m_dataSource->cumulatedHeightFromIndex(y), columnWidth(x), m_dataSource->rowHeight(y));
  m_tableView->scrollToContentRect(cellRect, true);
}

void TableView::ContentView::reloadCellAtLocation(int i, int j) {
  HighlightCell * cell = cellAtLocation(i, j);
  if (cell) {
    m_dataSource->willDisplayCellAtLocation(cellAtLocation(i, j), i, j);
  }
}

int TableView::ContentView::typeOfSubviewAtIndex(int index) const {
  assert(index >= 0);
  int i = absoluteColumnNumberFromSubviewIndex(index);
  int j = absoluteRowNumberFromSubviewIndex(index);
  int type = m_dataSource->typeAtLocation(i, j);
  return type;
}

int TableView::ContentView::typeIndexFromSubviewIndex(int index, int type) const {
  int typeIndex = 0;
  for (int k = 0; k < index; k++) {
    if (typeOfSubviewAtIndex(k) == type) {
      typeIndex++;
    }
  }
  assert(typeIndex < m_dataSource->reusableCellCount(type));
  return typeIndex;
}

HighlightCell * TableView::ContentView::cellAtLocation(int x, int y) {
  int relativeX = x-columnsScrollingOffset();
  int relativeY = y-rowsScrollingOffset();
  if (relativeY >= numberOfDisplayableRows() || relativeX >= numberOfDisplayableColumns()) {
    return nullptr;
  }
  int type = m_dataSource->typeAtLocation(x, y);
  int index = relativeY*numberOfDisplayableColumns()+relativeX;
  int typeIndex = typeIndexFromSubviewIndex(index, type);
  return m_dataSource->reusableCell(typeIndex, type);
}

#if ESCHER_VIEW_LOGGING
const char * TableView::ContentView::className() const {
  return "TableView::ContentView";
}
#endif

int TableView::ContentView::numberOfSubviews() const {
  return numberOfDisplayableRows() * numberOfDisplayableColumns();
}

int TableView::ContentView::absoluteColumnNumberFromSubviewIndex(int index) const {
  /* "x = i % columns" but we avoid a call to modulo not to implement
  * "__aeabi_idivmod" */
  int displayableColumnsCount = numberOfDisplayableColumns();
  int j = index / displayableColumnsCount;
  int i = index - j * displayableColumnsCount;
  int columnOffset = columnsScrollingOffset();
  return i + columnOffset;
}

int TableView::ContentView::absoluteRowNumberFromSubviewIndex(int index) const {
  int j = index / numberOfDisplayableColumns();
  int rowOffset = rowsScrollingOffset();
  return j + rowOffset;
}

View * TableView::ContentView::subviewAtIndex(int index) {
  int type = typeOfSubviewAtIndex(index);
  int typeIndex = typeIndexFromSubviewIndex(index, type);
  return m_dataSource->reusableCell(typeIndex, type);
}

void TableView::ContentView::layoutSubviews() {
  /* The number of subviews might change during the layouting so it needs to be
   * recomputed at each step of the for loop. */
  for (int index = 0; index < numberOfSubviews(); index++) {
    View * cell = subview(index);
    int i = absoluteColumnNumberFromSubviewIndex(index);
    int j = absoluteRowNumberFromSubviewIndex(index);
    m_dataSource->willDisplayCellAtLocation((HighlightCell *)cell, i, j);

    KDCoordinate rowHeight = m_dataSource->rowHeight(j);
    KDCoordinate columnWidth = this->columnWidth(i);
    KDCoordinate verticalOffset = m_dataSource->cumulatedHeightFromIndex(j);
    KDCoordinate horizontalOffset = m_dataSource->cumulatedWidthFromIndex(i);
    KDRect cellFrame(horizontalOffset, verticalOffset,
      columnWidth+m_horizontalCellOverlap, rowHeight+m_verticalCellOverlap);
    cell->setFrame(cellFrame);
  }
}


int TableView::ContentView::numberOfFullyDisplayableRows() const {
  // The number of displayable rows taking into accounts margins
  int rowOffsetWithMargin = m_dataSource->indexFromCumulatedHeight(m_tableView->contentOffset().y() +
    m_tableView->topMargin());
  int displayedHeightWithOffsetAndMargin = m_dataSource->indexFromCumulatedHeight(m_tableView->maxContentHeightDisplayableWithoutScrolling() +
    m_tableView->contentOffset().y() + m_tableView->topMargin());
  return displayedHeightWithOffsetAndMargin - rowOffsetWithMargin;
}

int TableView::ContentView::numberOfFullyDisplayableColumns() const {
  // The number of displayable rows taking into accounts margins
  int columnOffsetWithMargin = m_dataSource->indexFromCumulatedWidth(m_tableView->contentOffset().x() +
    m_tableView->leftMargin());
  int displayedWidthWithOffsetAndMargin = m_dataSource->indexFromCumulatedWidth(m_tableView->maxContentWidthDisplayableWithoutScrolling() +
    m_tableView->contentOffset().x() + m_tableView->leftMargin());
  return displayedWidthWithOffsetAndMargin - columnOffsetWithMargin;
}

int TableView::ContentView::numberOfDisplayableRows() const {
  int rowOffset = rowsScrollingOffset();
  int displayedHeightWithOffset = m_dataSource->indexFromCumulatedHeight(m_tableView->bounds().height() + m_tableView->contentOffset().y());
  return MIN(
    m_dataSource->numberOfRows(),
    displayedHeightWithOffset + 1
  )  - rowOffset;
}

int TableView::ContentView::numberOfDisplayableColumns() const {
  int columnOffset = columnsScrollingOffset();
  int displayedWidthWithOffset = m_dataSource->indexFromCumulatedWidth(m_tableView->bounds().width() + m_tableView->contentOffset().x());
  return MIN(
    m_dataSource->numberOfColumns(),
    displayedWidthWithOffset + 1
  )  - columnOffset;
}

int TableView::ContentView::rowsScrollingOffset() const {
  /* Here, we want to translate the offset at which our tableView is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  KDCoordinate invisibleHeight = m_tableView->contentOffset().y()-m_tableView->topMargin();
  invisibleHeight = invisibleHeight < 0 ? 0 : invisibleHeight;
  return m_dataSource->indexFromCumulatedHeight(invisibleHeight);
}

int TableView::ContentView::columnsScrollingOffset() const {
  /* Here, we want to translate the offset at which our tableView is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  KDCoordinate invisibleWidth = m_tableView->contentOffset().x()-m_tableView->leftMargin();
  invisibleWidth = invisibleWidth < 0 ? 0 : invisibleWidth;
  return m_dataSource->indexFromCumulatedWidth(invisibleWidth);
}
