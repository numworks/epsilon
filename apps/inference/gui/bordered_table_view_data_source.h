#ifndef PROBABILITY_GUI_BORDERED_TABLE_VIEW_DATA_SOURCE_H
#define PROBABILITY_GUI_BORDERED_TABLE_VIEW_DATA_SOURCE_H

#include <escher/metric.h>
#include <escher/table_view_data_source.h>

namespace Inference {

class BorderedTableViewDataSource : public Escher::TableViewDataSource {
public:
  virtual KDCoordinate verticalBorderWidth() { return 0; }
  virtual KDCoordinate horizontalBorderHeight() { return 0; }
  KDCoordinate cumulatedWidthFromIndex(int i) override {
    return Escher::TableViewDataSource::cumulatedWidthFromIndex(i) + (i == numberOfColumns() ? i - 1 : i) * verticalBorderWidth();
  }
  KDCoordinate cumulatedHeightFromIndex(int j) override {
    return Escher::TableViewDataSource::cumulatedHeightFromIndex(j) + j * horizontalBorderHeight();
  }
};

}  // namespace Inference

#endif /* PROBABILITY_GUI_BORDERED_TABLE_VIEW_DATA_SOURCE_H */
