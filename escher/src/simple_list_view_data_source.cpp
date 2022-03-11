#include <escher/simple_list_view_data_source.h>

namespace Escher {

KDCoordinate RegularListViewDataSource::cumulatedHeightFromIndex(int j) {
  return cellHeight() * j;
}

int RegularListViewDataSource::indexFromCumulatedHeight(KDCoordinate offsetY) {
  KDCoordinate height = cellHeight();
  if (height == 0) {
    return 0;
  }
  return (offsetY - 1) / height;
}

}
