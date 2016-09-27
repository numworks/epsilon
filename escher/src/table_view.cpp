#include <escher/table_view.h>
#include <escher/metric.h>

extern "C" {
#include <assert.h>
}

#define MIN(x,y) ((x)<(y) ? (x) : (y))

void TableViewDataSource::willDisplayCellAtLocation(View * cell, int x, int y) {
}


TableView::TableView(TableViewDataSource * dataSource, KDCoordinate topMargin, KDCoordinate rightMargin,
    KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  ScrollView(&m_contentView, topMargin, rightMargin, bottomMargin, leftMargin),
  m_contentView(TableView::ContentView(this, dataSource))
{
}

// This method computes the minimal scrolling needed to properly display the
// requested cell.
void TableView::scrollToCell(int x, int y) {
  m_contentView.scrollToCell(x, y);
}

View * TableView::cellAtIndex(int x, int y) {
  return m_contentView.cellAtIndex(x, y);
}

#if ESCHER_VIEW_LOGGING
const char * TableView::className() const {
  return "TableView";
}
#endif

void TableView::layoutSubviews() {
  // We only have to layout our contentView.
  // We will size it here, and ScrollView::layoutSubviews will position it.

  KDRect contentViewFrame(0, 0, m_contentView.width(), m_contentView.height());
  m_contentView.setFrame(contentViewFrame);

  ScrollView::layoutSubviews();
}

/* TableView::ContentView */

TableView::ContentView::ContentView(TableView * tableView, TableViewDataSource * dataSource) :
  View(),
  m_tableView(tableView),
  m_dataSource(dataSource)
{
}

KDCoordinate TableView::ContentView::height() const {
  return m_dataSource->numberOfRows() * m_dataSource->cellHeight();
}

KDCoordinate TableView::ContentView::width() const {
  return m_dataSource->numberOfColumns() * m_dataSource->cellWidth();
}

void TableView::ContentView::scrollToCell(int x, int y) const {
  KDCoordinate contentOffsetX = m_tableView->contentOffset().x();
  KDCoordinate contentOffsetY = m_tableView->contentOffset().y();
  if (columnAtIndexIsBeforeFullyVisibleRange(x)) {
    // Let's scroll the tableView to put that cell on the left (while keeping the left margin)
    contentOffsetX = x*m_dataSource->cellWidth();
  } else if (columnAtIndexIsAfterFullyVisibleRange(x)) {
    // Let's scroll the tableView to put that cell on the right (while keeping the right margin)
    contentOffsetX = (x+1)*m_dataSource->cellWidth() - m_tableView->maxContentWidthDisplayableWithoutScrolling();
  }
  if (rowAtIndexIsBeforeFullyVisibleRange(y)) {
    // Let's scroll the tableView to put that cell on the top (while keeping the top margin)
    contentOffsetY = y*m_dataSource->cellHeight();
  } else if (rowAtIndexIsAfterFullyVisibleRange(y)) {
    // Let's scroll the tableView to put that cell on the bottom (while keeping the bottom margin)
    contentOffsetY = (y+1)*m_dataSource->cellHeight() - m_tableView->maxContentHeightDisplayableWithoutScrolling();
  }
  m_tableView->setContentOffset(KDPoint(contentOffsetX, contentOffsetY));
}

View * TableView::ContentView::cellAtIndex(int x, int y) {
  int relativeX = x-columnsScrollingOffset();
  int relativeY = y-rowsScrollingOffset();
  return m_dataSource->reusableCell(relativeY*numberOfDisplayableColumns()+relativeX);
}

#if ESCHER_VIEW_LOGGING
const char * TableView::ContentView::className() const {
  return "TableView::ContentView";
}
#endif

int TableView::ContentView::numberOfSubviews() const {
  int result = numberOfDisplayableRows() * numberOfDisplayableColumns();
  assert(result <= m_dataSource->reusableCellCount());
  return result;
}

View * TableView::ContentView::subviewAtIndex(int index) {
  assert(index >= 0);
  assert(index < m_dataSource->reusableCellCount());
  return m_dataSource->reusableCell(index);
}

void TableView::ContentView::layoutSubviews() {
  int rowOffset = rowsScrollingOffset();
  int columnOffset = columnsScrollingOffset();

  for (int i=0; i<numberOfSubviews(); i++) {
    View * cell = subview(i);
    int columns = numberOfDisplayableColumns();
    int y = i / columns;
    /* "x = i % columns" but we avoid a call to modulo not to implement
     * "__aeabi_idivmod" */
    int x = i - y * columns;

    KDCoordinate cellHeight = m_dataSource->cellHeight();
    KDCoordinate cellWidth = m_dataSource->cellWidth();
    KDRect cellFrame((columnOffset+x)*cellWidth, (rowOffset+y)*cellHeight,
      cellWidth, cellHeight);

    cell->setFrame(cellFrame);

    m_dataSource->willDisplayCellAtLocation(cell, columnOffset+x, rowOffset+y);
  }
}

int TableView::ContentView::numberOfFullyDisplayableRows() const {
  // The number of displayable rows taking into accounts margins
  return m_tableView->maxContentHeightDisplayableWithoutScrolling()/m_dataSource->cellHeight();
}

int TableView::ContentView::numberOfFullyDisplayableColumns() const {
  // The number of displayable columns taking into accounts margins
  return m_tableView->maxContentWidthDisplayableWithoutScrolling()/m_dataSource->cellHeight();
}

int TableView::ContentView::numberOfDisplayableRows() const {
  return MIN(
    m_dataSource->numberOfRows(),
    m_tableView->bounds().height()/m_dataSource->cellHeight() + 2
  );
}

int TableView::ContentView::numberOfDisplayableColumns() const {
  return MIN(
    m_dataSource->numberOfColumns(),
    m_tableView->bounds().width()/m_dataSource->cellWidth() + 2
  );
}

int TableView::ContentView::rowsScrollingOffset() const {
  /* Here, we want to translate the offset at which our tableView is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  KDCoordinate pixelScrollingOffset = -m_frame.y();
  return pixelScrollingOffset / m_dataSource->cellHeight();
}

int TableView::ContentView::columnsScrollingOffset() const {
  /* Here, we want to translate the offset at which our tableView is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  KDCoordinate pixelScrollingOffset = -m_frame.x();
  return pixelScrollingOffset / m_dataSource->cellWidth();
}

bool TableView::ContentView::rowAtIndexIsBeforeFullyVisibleRange(int index) const {
  return index <= rowsScrollingOffset();
}

bool TableView::ContentView::columnAtIndexIsBeforeFullyVisibleRange(int index) const {
  return index <= columnsScrollingOffset();
}

bool TableView::ContentView::rowAtIndexIsAfterFullyVisibleRange(int index) const {
  int relativeIndex = index - rowsScrollingOffset();
  return (relativeIndex >= numberOfFullyDisplayableRows());
}

bool TableView::ContentView::columnAtIndexIsAfterFullyVisibleRange(int index) const {
  int relativeIndex = index - columnsScrollingOffset();
  return (relativeIndex >= numberOfFullyDisplayableColumns());
}
