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

void ListViewDataSource::willDisplayCellAtLocation(HighlightCell * cell, int x, int y) {
  willDisplayCellForIndex(cell, y);
}

void ListViewDataSource::willDisplayCellForIndex(HighlightCell * cell, int index) {
}

KDCoordinate ListViewDataSource::cumulatedWidthFromIndex(int i) {
  if (i == 1) {
    return cellWidth();
  }
  return 0;
}

int ListViewDataSource::indexFromCumulatedWidth(KDCoordinate offsetX) {
  return 0;
}
