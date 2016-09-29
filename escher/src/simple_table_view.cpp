#include <escher/simple_table_view.h>
#include <escher/metric.h>

extern "C" {
#include <assert.h>
}

#define MIN(x,y) ((x)<(y) ? (x) : (y))

void SimpleTableViewDataSource::willDisplayCellAtLocation(View * cell, int x, int y) {
}


SimpleTableView::SimpleTableView(SimpleTableViewDataSource * dataSource, KDCoordinate topMargin, KDCoordinate rightMargin,
    KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  ScrollView(&m_contentView, topMargin, rightMargin, bottomMargin, leftMargin),
  m_contentView(SimpleTableView::ContentView(this, dataSource))
{
}

// This method computes the minimal scrolling needed to properly display the
// requested cell.
void SimpleTableView::scrollToCell(int x, int y) {
  m_contentView.scrollToCell(x, y);
}

View * SimpleTableView::cellAtLocation(int x, int y) {
  return m_contentView.cellAtLocation(x, y);
}

#if ESCHER_VIEW_LOGGING
const char * SimpleTableView::className() const {
  return "SimpleTableView";
}
#endif

void SimpleTableView::layoutSubviews() {
  // We only have to layout our contentView.
  // We will size it here, and ScrollView::layoutSubviews will position it.

  KDRect contentViewFrame(0, 0, m_contentView.width(), m_contentView.height());
  m_contentView.setFrame(contentViewFrame);

  ScrollView::layoutSubviews();
}

/* SimpleTableView::ContentView */

SimpleTableView::ContentView::ContentView(SimpleTableView * tableView, SimpleTableViewDataSource * dataSource) :
  View(),
  m_tableView(tableView),
  m_dataSource(dataSource)
{
}

KDCoordinate SimpleTableView::ContentView::realCellWidth() const {
  int cellWidth = m_dataSource->cellWidth();
  cellWidth = cellWidth ? cellWidth : m_tableView->maxContentWidthDisplayableWithoutScrolling();
  return cellWidth;
}


KDCoordinate SimpleTableView::ContentView::height() const {
  return m_dataSource->numberOfRows() * m_dataSource->cellHeight();
}

KDCoordinate SimpleTableView::ContentView::width() const {
  return m_dataSource->numberOfColumns() * realCellWidth();
}

void SimpleTableView::ContentView::scrollToCell(int x, int y) const {
  KDCoordinate contentOffsetX = m_tableView->contentOffset().x();
  KDCoordinate contentOffsetY = m_tableView->contentOffset().y();
  if (columnAtIndexIsBeforeFullyVisibleRange(x)) {
    // Let's scroll the tableView to put that cell on the left (while keeping the left margin)
    contentOffsetX = x*realCellWidth();
  } else if (columnAtIndexIsAfterFullyVisibleRange(x)) {
    // Let's scroll the tableView to put that cell on the right (while keeping the right margin)
    contentOffsetX = (x+1)*realCellWidth() - m_tableView->maxContentWidthDisplayableWithoutScrolling();
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

View * SimpleTableView::ContentView::cellAtLocation(int x, int y) {
  int relativeX = x-columnsScrollingOffset();
  int relativeY = y-rowsScrollingOffset();
  return m_dataSource->reusableCell(relativeY*numberOfDisplayableColumns()+relativeX);
}

#if ESCHER_VIEW_LOGGING
const char * SimpleTableView::ContentView::className() const {
  return "TableView::ContentView";
}
#endif

int SimpleTableView::ContentView::numberOfSubviews() const {
  int result = numberOfDisplayableRows() * numberOfDisplayableColumns();
  assert(result <= m_dataSource->reusableCellCount());
  return result;
}

View * SimpleTableView::ContentView::subviewAtIndex(int index) {
  assert(index >= 0);
  assert(index < m_dataSource->reusableCellCount());
  return m_dataSource->reusableCell(index);
}

void SimpleTableView::ContentView::layoutSubviews() {
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
    KDCoordinate cellWidth = realCellWidth();
    KDRect cellFrame((columnOffset+x)*cellWidth, (rowOffset+y)*cellHeight,
      cellWidth, cellHeight);

    cell->setFrame(cellFrame);

    m_dataSource->willDisplayCellAtLocation(cell, columnOffset+x, rowOffset+y);
  }
}

int SimpleTableView::ContentView::numberOfFullyDisplayableRows() const {
  // The number of displayable rows taking into accounts margins
  return m_tableView->maxContentHeightDisplayableWithoutScrolling()/m_dataSource->cellHeight();
}

int SimpleTableView::ContentView::numberOfFullyDisplayableColumns() const {
  // The number of displayable columns taking into accounts margins
  return m_tableView->maxContentWidthDisplayableWithoutScrolling()/m_dataSource->cellHeight();
}

int SimpleTableView::ContentView::numberOfDisplayableRows() const {
  return MIN(
    m_dataSource->numberOfRows(),
    m_tableView->bounds().height() / m_dataSource->cellHeight() + 2
  );
}

int SimpleTableView::ContentView::numberOfDisplayableColumns() const {
  KDCoordinate width = realCellWidth();
  if (width == 0) {
    return 0;
  }
  return MIN(
    m_dataSource->numberOfColumns(),
    m_tableView->bounds().width() / width + 2
  );
}

int SimpleTableView::ContentView::rowsScrollingOffset() const {
  /* Here, we want to translate the offset at which our tableView is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  KDCoordinate pixelScrollingOffset = -m_frame.y();
  return pixelScrollingOffset / m_dataSource->cellHeight();
}

int SimpleTableView::ContentView::columnsScrollingOffset() const {
  /* Here, we want to translate the offset at which our tableView is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  KDCoordinate width = realCellWidth();
  if (width == 0) {
    return 0;
  }
  KDCoordinate pixelScrollingOffset = -m_frame.x();
  return pixelScrollingOffset / width;
}

bool SimpleTableView::ContentView::rowAtIndexIsBeforeFullyVisibleRange(int index) const {
  return index <= rowsScrollingOffset();
}

bool SimpleTableView::ContentView::columnAtIndexIsBeforeFullyVisibleRange(int index) const {
  return index <= columnsScrollingOffset();
}

bool SimpleTableView::ContentView::rowAtIndexIsAfterFullyVisibleRange(int index) const {
  int relativeIndex = index - rowsScrollingOffset();
  return (relativeIndex >= numberOfFullyDisplayableRows());
}

bool SimpleTableView::ContentView::columnAtIndexIsAfterFullyVisibleRange(int index) const {
  int relativeIndex = index - columnsScrollingOffset();
  return (relativeIndex >= numberOfFullyDisplayableColumns());
}
