#include <escher/list_view.h>
#include <escher/metric.h>

extern "C" {
#include <assert.h>
}

#define MIN(x,y) ((x)<(y) ? (x) : (y))

void ListViewDataSource::willDisplayCellForIndex(View * cell, int index) {
}


ListView::ListView(ListViewDataSource * dataSource, KDCoordinate topMargin, KDCoordinate rightMargin,
    KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  ScrollView(&m_contentView, topMargin, rightMargin, bottomMargin, leftMargin),
  m_contentView(ListView::ContentView(this, dataSource))
{
}

// This method computes the minimal scrolling needed to properly display the
// requested cell.
void ListView::scrollToRow(int index) {
  m_contentView.scrollToRow(index);
}

View * ListView::cellAtIndex(int index) {
  return m_contentView.cellAtIndex(index);
}

#if ESCHER_VIEW_LOGGING
const char * ListView::className() const {
  return "ListView";
}
#endif

void ListView::layoutSubviews() {
  // We only have to layout our contentView.
  // We will size it here, and ScrollView::layoutSubviews will position it.

  KDRect contentViewFrame(0, 0, maxContentWidthDisplayableWithoutScrolling(), m_contentView.height());
  m_contentView.setFrame(contentViewFrame);

  ScrollView::layoutSubviews();
}

/* ListView::ContentView */

ListView::ContentView::ContentView(ListView * listView, ListViewDataSource * dataSource) :
  View(),
  m_listView(listView),
  m_dataSource(dataSource)
{
}

KDCoordinate ListView::ContentView::height() const {
  return m_dataSource->numberOfCells() * m_dataSource->cellHeight();
}

void ListView::ContentView::scrollToRow(int index) const {
  if (cellAtIndexIsBeforeFullyVisibleRange(index)) {
    // Let's scroll the listView to put that cell on the top (while keeping the top margin)
    KDPoint contentOffset(0, index*m_dataSource->cellHeight());
    m_listView->setContentOffset(contentOffset);
    return;
  }
  if (cellAtIndexIsAfterFullyVisibleRange(index)) {
    // Let's scroll the listView to put that cell on the bottom (while keeping the bottom margin)
    KDPoint contentOffset(0,
    (index+1)*m_dataSource->cellHeight() - m_listView->maxContentHeightDisplayableWithoutScrolling());
    m_listView->setContentOffset(contentOffset);
    return;
  }
  // Nothing to do if the cell is already visible!
}

View * ListView::ContentView::cellAtIndex(int index) {
  return m_dataSource->reusableCell(index - cellScrollingOffset());
}

#if ESCHER_VIEW_LOGGING
const char * ListView::ContentView::className() const {
  return "ListView::ContentView";
}
#endif

int ListView::ContentView::numberOfSubviews() const {
  int result = MIN(m_dataSource->numberOfCells(), numberOfDisplayableCells());
  assert(result <= m_dataSource->reusableCellCount());
  return result;
}

View * ListView::ContentView::subviewAtIndex(int index) {
  assert(index >= 0);
  assert(index < m_dataSource->reusableCellCount());
  return m_dataSource->reusableCell(index);
}

void ListView::ContentView::layoutSubviews() {
  int cellOffset = cellScrollingOffset();

  for (int i=0; i<numberOfSubviews(); i++) {
    View * cell = subview(i);

    KDCoordinate cellHeight = m_dataSource->cellHeight();
    KDRect cellFrame(0, (cellOffset+i)*cellHeight, m_frame.width(), cellHeight);

    cell->setFrame(cellFrame);

    m_dataSource->willDisplayCellForIndex(cell, cellOffset+i);
  }
}

int ListView::ContentView::numberOfFullyDisplayableCells() const {
  /* This function considers that cells in top and bottom margins are not
  * "fully" displayed. */
  return (m_listView->maxContentHeightDisplayableWithoutScrolling()) / m_dataSource->cellHeight() + 1;
}

int ListView::ContentView::numberOfDisplayableCells() const {
  return (m_listView->bounds().height()) / m_dataSource->cellHeight() + 1;
}

int ListView::ContentView::cellScrollingOffset() const {
  /* Here, we want to translate the offset at which our listView is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  KDCoordinate pixelScrollingOffset = -m_frame.y();
  return pixelScrollingOffset / m_dataSource->cellHeight();
}

bool ListView::ContentView::cellAtIndexIsBeforeFullyVisibleRange(int index) const {
  return index <= cellScrollingOffset();
}

bool ListView::ContentView::cellAtIndexIsAfterFullyVisibleRange(int index) const {
  int relativeIndex = index - cellScrollingOffset();
  return (relativeIndex >= numberOfFullyDisplayableCells()-1);
}
