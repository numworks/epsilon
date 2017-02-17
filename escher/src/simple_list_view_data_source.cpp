#include <escher/simple_list_view_data_source.h>
#include <assert.h>

KDCoordinate SimpleListViewDataSource::rowHeight(int j) {
  return cellHeight();
}

KDCoordinate SimpleListViewDataSource::cumulatedHeightFromIndex(int j) {
  return cellHeight() * j;
}

int SimpleListViewDataSource::indexFromCumulatedHeight(KDCoordinate offsetY) {
  KDCoordinate height = cellHeight();
  if (height == 0) {
    return 0;
  }
  return (offsetY - 1) / height;
}

HighlightCell * SimpleListViewDataSource::reusableCell(int index, int type) {
  assert(type == 0);
  return reusableCell(index);
}

int SimpleListViewDataSource::reusableCellCount(int type) {
  assert(type == 0);
  return reusableCellCount();
}

int SimpleListViewDataSource::typeAtLocation(int i, int j) {
  return 0;
}
