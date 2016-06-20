#include <escher/table_view.h>
extern "C" {
#include <assert.h>
}

#define MIN(x,y) ((x)<(y) ? (x) : (y))

TableView::TableView(TableViewDataSource * dataSource) :
  ScrollView(&m_contentView),
  m_contentView(TableView::ContentView(this, dataSource))
{
}

// This method computes the minimal scrolling needed to properly display the
// requested cell.
void TableView::scrollToRow(int index) {
  m_contentView.scrollToRow(index);
}

void TableView::layoutSubviews() {
  // We only have to layout our contentView.
  // We will size it here, and ScrollView::layoutSubviews will position it.

  KDRect contentViewFrame;
  contentViewFrame.origin = KDPointZero; // Will be overriden by ScrollView::layoutSubviews
  contentViewFrame.width = maxContentWidth();
  contentViewFrame.height = m_contentView.height();
  m_contentView.setFrame(contentViewFrame);

  ScrollView::layoutSubviews();
}

View * TableView::cellAtIndex(int index) {
  return m_contentView.cellAtIndex(index);
}

#if ESCHER_VIEW_LOGGING
const char * TableView::className() const {
  return "TableView";
}
#endif

/* TableView::ContentView */

TableView::ContentView::ContentView(TableView * tableView, TableViewDataSource * dataSource) :
  View(),
  m_tableView(tableView),
  m_dataSource(dataSource)
{
}

int TableView::ContentView::numberOfSubviews() const {
  return MIN(m_dataSource->numberOfCells(), numberOfDisplayableCells());
}

View * TableView::ContentView::subviewAtIndex(int index) {
  assert(index >= 0);
  assert(index < m_dataSource->reusableCellCount());
  return m_dataSource->reusableCell(index);
}

void TableView::ContentView::layoutSubviews() {
  int cellOffset = cellScrollingOffset();

  for (int i=0; i<numberOfSubviews(); i++) {
    View * cell = subview(i);

    KDRect cellFrame;
    KDCoordinate cellHeight = m_dataSource->cellHeight();
    cellFrame.x = 0;
    cellFrame.y = (cellOffset+i)*cellHeight;
    cellFrame.width = m_frame.width;
    cellFrame.height = cellHeight;

    cell->setFrame(cellFrame);

    m_dataSource->willDisplayCellForIndex(cell, cellOffset+i);
  }
}

View * TableView::ContentView::cellAtIndex(int index) {
  return m_dataSource->reusableCell(index - cellScrollingOffset());
}

KDCoordinate TableView::ContentView::height() const {
  return m_dataSource->numberOfCells() * m_dataSource->cellHeight();
}

void TableView::ContentView::scrollToRow(int index) const {
  if (cellAtIndexIsBeforeFullyVisibleRange(index)) {
    // Let's scroll the tableView to put that cell on the top
    KDPoint contentOffset;
    contentOffset.x = 0;
    contentOffset.y = index*m_dataSource->cellHeight();
    m_tableView->setContentOffset(contentOffset);
    return;
  }
  if (cellAtIndexIsAfterFullyVisibleRange(index)) {
    // Let's scroll the tableView to put that cell on the bottom
    KDPoint contentOffset;
    contentOffset.x = 0;
    contentOffset.y = (index+1)*m_dataSource->cellHeight() - m_tableView->bounds().height;
    m_tableView->setContentOffset(contentOffset);
    return;
  }
  // Nothing to do if the cell is already visible!
}

#if ESCHER_VIEW_LOGGING
const char * TableView::ContentView::className() const {
  return "TableView::ContentView";
}
#endif

int TableView::ContentView::numberOfDisplayableCells() const {
  int result = m_tableView->bounds().height / m_dataSource->cellHeight() + 1;
  assert(result <= m_dataSource->reusableCellCount());
  return result;
}

int TableView::ContentView::cellScrollingOffset() const {
  /* Here, we want to translate the offset at which our tableView is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  KDCoordinate pixelScrollingOffset = -m_frame.y;
  return pixelScrollingOffset / m_dataSource->cellHeight();
}

bool TableView::ContentView::cellAtIndexIsBeforeFullyVisibleRange(int index) const {
  return index <= cellScrollingOffset();
}

bool TableView::ContentView::cellAtIndexIsAfterFullyVisibleRange(int index) const {
  int relativeIndex = index - cellScrollingOffset();
  return (relativeIndex >= numberOfDisplayableCells()-1);
}
