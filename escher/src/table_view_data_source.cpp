#include <escher/table_view_data_source.h>

namespace Escher {

void TableViewDataSource::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
}

KDCoordinate TableViewDataSource::cumulatedWidthFromIndex(int i) {
  int result = 0;
  for (int k = 0; k < i; k++) {
    result += columnWidth(k);
  }
  return result;
}

KDCoordinate TableViewDataSource::cumulatedHeightFromIndex(int j) {
  int result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
  }
  return result;
}

int TableViewDataSource::indexFromCumulatedWidth(KDCoordinate offsetX) {
  int result = 0;
  int i = 0;
  while (result < offsetX && i < numberOfColumns()) {
    result += columnWidth(i++);
  }
  return (result < offsetX || offsetX == 0) ? i : i - 1;
}

int TableViewDataSource::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  int nRows = numberOfRows();
  while (result < offsetY && j < nRows) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

}
