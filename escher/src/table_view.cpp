#include <escher/table_view.h>
extern "C" {
#include <assert.h>
}

#define MIN(x,y) ((x)<(y) ? (x) : (y))

TableView::TableView(TableViewDataSource * dataSource) :
  ScrollView(&m_contentView),
  m_contentView(TableView::ContentView(dataSource))
{
  setSubview(&m_contentView, 0);
}

// This method computes the minimal scrolling needed to properly display the
// requested cell.
void TableView::scrollToRow(int index) {
  m_contentView.scrollToRow(index);
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

TableView::ContentView::ContentView(TableViewDataSource * dataSource) :
  View(),
  m_dataSource(dataSource)
{
  for (int i=0; i<m_dataSource->reusableCellCount(); i++) {
    setSubview(m_dataSource->reusableCell(i), i);
  }
}

int TableView::ContentView::numberOfSubviews() const {
  return MIN(m_dataSource->numberOfCells(), numberOfDisplayableCells());
}

View * TableView::ContentView::subview(int index) {
  assert(index >= 0);
  assert(index < m_dataSource->reusableCellCount());
  return m_dataSource->reusableCell(index);
}

void TableView::ContentView::storeSubviewAtIndex(View * view, int index) {
  // Do nothing!
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

void TableView::ContentView::scrollToRow(int index) const {
  TableView * superview = (TableView *)m_superview;
  if (cellAtIndexIsBeforeFullyVisibleRange(index)) {
    // Let's scroll the tableView to put that cell on the top
    KDPoint contentOffset;
    contentOffset.x = 0;
    contentOffset.y = index*m_dataSource->cellHeight();
    superview->setContentOffset(contentOffset);
    return;
  }
  if (cellAtIndexIsAfterFullyVisibleRange(index)) {
    // Let's scroll the tableView to put that cell on the bottom
    KDPoint contentOffset;
    contentOffset.x = 0;
    contentOffset.y = (index+1)*m_dataSource->cellHeight() - superview->bounds().height;
    superview->setContentOffset(contentOffset);
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
  int result = m_superview->bounds().height / m_dataSource->cellHeight() + 1;
  assert(result <= m_dataSource->reusableCellCount());
  return result;
}

int TableView::ContentView::cellScrollingOffset() const {
  /* Here, we want to translate the offset at which our superview is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  KDCoordinate pixelScrollingOffset = -m_frame.y;
  return pixelScrollingOffset / m_dataSource->cellHeight();
}

bool TableView::ContentView::cellAtIndexIsBeforeFullyVisibleRange(int index) const {
  return index < cellScrollingOffset();
}

bool TableView::ContentView::cellAtIndexIsAfterFullyVisibleRange(int index) const {
  int relativeIndex = index - cellScrollingOffset();
  return (relativeIndex >= numberOfDisplayableCells()-1);
}
