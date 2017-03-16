#ifndef SHARED_REGULAR_TABLE_VIEW_DATA_SOURCE_H
#define SHARED_REGULAR_TABLE_VIEW_DATA_SOURCE_H

#include <escher.h>

namespace Shared {

/* Regular table view data source is a table view data source with equal sized
 * columns and equal sized rows */

class RegularTableViewDataSource : public TableViewDataSource {
public:
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
};

}

#endif

