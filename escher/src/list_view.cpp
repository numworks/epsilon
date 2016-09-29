#include <escher/list_view.h>
#include <escher/metric.h>

extern "C" {
#include <assert.h>
}

#define MIN(x,y) ((x)<(y) ? (x) : (y))

int ListViewDataSource::numberOfColumns() {
  return 1;
}

KDCoordinate ListViewDataSource::cellWidth() {
  return 0;
}

void ListViewDataSource::willDisplayCellAtLocation(View * cell, int x, int y) {
  willDisplayCellForIndex(cell, y);
}

void ListViewDataSource::willDisplayCellForIndex(View * cell, int index) {
}

ListView::ListView(ListViewDataSource * dataSource, KDCoordinate topMargin, KDCoordinate rightMargin,
    KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  SimpleTableView(dataSource, topMargin, rightMargin, bottomMargin, leftMargin)
{
}

// This method computes the minimal scrolling needed to properly display the
// requested cell.
void ListView::scrollToRow(int index) {
  scrollToCell(0, index);
}

View * ListView::cellAtIndex(int index) {
  return cellAtLocation(0, index);
}

#if ESCHER_VIEW_LOGGING
const char * ListView::className() const {
  return "ListView";
}
#endif
