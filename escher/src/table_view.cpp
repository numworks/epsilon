#include <escher/table_view.h>
#include <escher/metric.h>

extern "C" {
#include <assert.h>
}

#define MIN(x,y) ((x)<(y) ? (x) : (y))

TableView::TableView(TableViewDataSource * dataSource, KDCoordinate topMargin, KDCoordinate rightMargin,
    KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  ScrollView(&m_contentView, topMargin, rightMargin, bottomMargin, leftMargin),
  m_contentView(TableView::ContentView(this, dataSource))
{
}

// This method computes the minimal scrolling needed to properly display the
// requested cell.
void TableView::scrollToCell(int i, int j) {
  m_contentView.scrollToCell(i, j);
}

TableViewCell * TableView::cellAtLocation(int i, int j) {
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

void TableView::reloadData() {
  layoutSubviews();
}

/* TableView::ContentView */

TableView::ContentView::ContentView(TableView * tableView, TableViewDataSource * dataSource) :
  View(),
  m_tableView(tableView),
  m_dataSource(dataSource)
{
}

KDCoordinate TableView::ContentView::columnWidth(int i) const {
  int columnWidth = m_dataSource->columnWidth(i);
  columnWidth = columnWidth ? columnWidth : m_tableView->maxContentWidthDisplayableWithoutScrolling();
  return columnWidth;
}

void TableView::ContentView::resizeToFitContent() {
  setSize(KDSize(width(), height()));
  layoutSubviews();
}

KDCoordinate TableView::ContentView::height() const {
  return m_dataSource->cumulatedHeightFromIndex(m_dataSource->numberOfRows());
}

KDCoordinate TableView::ContentView::width() const {
  int result = m_dataSource->cumulatedWidthFromIndex(m_dataSource->numberOfColumns());
  // handle the case of list: cumulatedWidthFromIndex() = 0
  return result ? result : m_tableView->maxContentWidthDisplayableWithoutScrolling();
}

void TableView::ContentView::scrollToCell(int x, int y) const {
  KDCoordinate contentOffsetX = m_tableView->contentOffset().x();
  KDCoordinate contentOffsetY = m_tableView->contentOffset().y();
  if (columnAtIndexIsBeforeFullyVisibleRange(x)) {
    // Let's scroll the tableView to put that cell on the left (while keeping the left margin)
    contentOffsetX = m_dataSource->cumulatedWidthFromIndex(x);
  } else if (columnAtIndexIsAfterFullyVisibleRange(x)) {
    // Let's scroll the tableView to put that cell on the right (while keeping the right margin)
    contentOffsetX = m_dataSource->cumulatedWidthFromIndex(x+1)-m_tableView->maxContentWidthDisplayableWithoutScrolling();
  }
  if (rowAtIndexIsBeforeFullyVisibleRange(y)) {
    // Let's scroll the tableView to put that cell on the top (while keeping the top margin)
    contentOffsetY = m_dataSource->cumulatedHeightFromIndex(y);
  } else if (rowAtIndexIsAfterFullyVisibleRange(y)) {
    // Let's scroll the tableView to put that cell on the bottom (while keeping the bottom margin)
    contentOffsetY = m_dataSource->cumulatedHeightFromIndex(y+1) - m_tableView->maxContentHeightDisplayableWithoutScrolling();
  }
  m_tableView->setContentOffset(KDPoint(contentOffsetX, contentOffsetY));
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

TableViewCell * TableView::ContentView::cellAtLocation(int x, int y) {
  int relativeX = x-columnsScrollingOffset();
  int relativeY = y-rowsScrollingOffset();
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
  int result = numberOfDisplayableRows() * numberOfDisplayableColumns();
  return result;
}

int TableView::ContentView::absoluteColumnNumberFromSubviewIndex(int index) const {
  /* "x = i % columns" but we avoid a call to modulo not to implement
  * "__aeabi_idivmod" */
  int j = index / numberOfDisplayableColumns();
  int i = index - j * numberOfDisplayableColumns();
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
  for (int index=0; index<numberOfSubviews(); index++) {
    View * cell = subview(index);
    int i = absoluteColumnNumberFromSubviewIndex(index);
    int j = absoluteRowNumberFromSubviewIndex(index);

    KDCoordinate rowHeight = m_dataSource->rowHeight(j);
    KDCoordinate columnWidth = this->columnWidth(i);
    KDCoordinate verticalOffset = m_dataSource->cumulatedHeightFromIndex(j);
    KDCoordinate horizontalOffset = m_dataSource->cumulatedWidthFromIndex(i);
    KDRect cellFrame(horizontalOffset, verticalOffset,
      columnWidth, rowHeight);

    cell->setFrame(cellFrame);

    m_dataSource->willDisplayCellAtLocation((TableViewCell *)cell, i, j);
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
  return m_dataSource->indexFromCumulatedHeight(m_tableView->contentOffset().y());
}

int TableView::ContentView::columnsScrollingOffset() const {
  /* Here, we want to translate the offset at which our tableView is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  return m_dataSource->indexFromCumulatedWidth(m_tableView->contentOffset().x());
}

bool TableView::ContentView::rowAtIndexIsBeforeFullyVisibleRange(int index) const {
  return index <= rowsScrollingOffset();
}

bool TableView::ContentView::columnAtIndexIsBeforeFullyVisibleRange(int index) const {
  return index <= columnsScrollingOffset();
}

bool TableView::ContentView::rowAtIndexIsAfterFullyVisibleRange(int index) const {
  int minHeightToDisplayRowAtIndex = m_dataSource->cumulatedHeightFromIndex(index+1);
  int heightToTheBottomOfTheScreen = m_tableView->contentOffset().y()+m_tableView->maxContentHeightDisplayableWithoutScrolling()+m_tableView->topMargin();
  return minHeightToDisplayRowAtIndex >= heightToTheBottomOfTheScreen;
}

bool TableView::ContentView::columnAtIndexIsAfterFullyVisibleRange(int index) const {
  int minWidthToDisplayColumnAtIndex = m_dataSource->cumulatedWidthFromIndex(index+1);
  int widthToTheRightOfTheScreen = m_tableView->contentOffset().x()+m_tableView->maxContentWidthDisplayableWithoutScrolling()+m_tableView->leftMargin();
  return minWidthToDisplayColumnAtIndex >= widthToTheRightOfTheScreen;
}
