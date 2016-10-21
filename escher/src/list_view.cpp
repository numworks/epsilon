#include <escher/list_view.h>
#include <escher/metric.h>

ListView::ListView(ListViewDataSource * dataSource, KDCoordinate topMargin, KDCoordinate rightMargin,
    KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  TableView(dataSource, topMargin, rightMargin, bottomMargin, leftMargin)
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
