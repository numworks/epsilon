#include <escher/table_view.h>
extern "C" {
#include <assert.h>
}

#define MIN(x,y) ((x)>(y) ? (x) : (y))

TableView::TableView(TableViewDataSource * dataSource) :
  ScrollView(&m_contentView),
  m_contentView(TableView::ContentView(dataSource))
{
}

void TableView::layoutSubviews() {
  KDRect frame;
  frame.x = 0;
  frame.y = 0;
  frame.width = m_frame.width;
  frame.height = 9999; // Depends on the datasource, actually...
  m_contentView.setFrame(frame);

  ScrollView::layoutSubviews();
}

/* TableView::ContentView */

TableView::ContentView::ContentView(TableViewDataSource * dataSource) :
  m_dataSource(dataSource)
{
}

int TableView::ContentView::numberOfSubviews() const {
  return MIN(m_dataSource->numberOfCells(), numberOfDisplayableCells());
}

View * TableView::ContentView::subview(int index) {
  int offset = cellScrollingOffset();
  View * cell = m_dataSource->reusableCell(index);
  m_dataSource->willDisplayCellForIndex(cell, offset+index);
  return cell;
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
  }
}

int TableView::ContentView::numberOfDisplayableCells() const {
  return m_frame.height / m_dataSource->cellHeight();
}

// Index of the topmost cell
int TableView::ContentView::cellScrollingOffset() {
  /* Here, we want to translate the offset at which our superview is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  KDCoordinate pixelScrollingOffset = m_frame.y;

  return pixelScrollingOffset / m_dataSource->cellHeight();
}
