#include "regular_table_view_data_source.h"

namespace Shared {

KDCoordinate RegularTableViewDataSource::cumulatedWidthFromIndex(int i) {
  return i*columnWidth(0);
}

KDCoordinate RegularTableViewDataSource::cumulatedHeightFromIndex(int j) {
  return j*rowHeight(0);
}

int RegularTableViewDataSource::indexFromCumulatedWidth(KDCoordinate offsetX) {
  return (offsetX-1) / columnWidth(0);
}

int RegularTableViewDataSource::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return (offsetY-1) / rowHeight(0);
}

}

