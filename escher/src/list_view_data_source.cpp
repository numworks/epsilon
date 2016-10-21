#include <escher/list_view_data_source.h>

KDCoordinate ListViewDataSource::cellWidth() {
  return 0;
}

KDCoordinate ListViewDataSource::columnWidth(int i) {
  return cellWidth();
}

int ListViewDataSource::numberOfColumns() {
  return 1;
}

void ListViewDataSource::willDisplayCellAtLocation(View * cell, int x, int y) {
  willDisplayCellForIndex(cell, y);
}

void ListViewDataSource::willDisplayCellForIndex(View * cell, int index) {
}

KDCoordinate ListViewDataSource::cumulatedWidthFromIndex(int i) {
  return 0;
}

int ListViewDataSource::indexFromCumulatedWidth(KDCoordinate offsetX) {
  return 0;
}
