#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_BORDERED_TABLE_VIEW_DATA_SOURCE_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_BORDERED_TABLE_VIEW_DATA_SOURCE_H

#include <escher/metric.h>
#include <escher/table_view_data_source.h>

namespace Inference {

class BorderedTableViewDataSource : public Escher::TableViewDataSource {
public:
  virtual KDCoordinate verticalBorderWidth() { return 0; }
  virtual KDCoordinate horizontalBorderHeight() { return 0; }
private:
  KDCoordinate nonMemoizedCumulatedWidthFromIndex(int i) override {
    return Escher::TableViewDataSource::nonMemoizedCumulatedWidthFromIndex(i) + (i == numberOfColumns() && i > 0 ? i - 1 : i) * verticalBorderWidth();
  }
  KDCoordinate nonMemoizedCumulatedHeightFromIndex(int j) override {
    return Escher::TableViewDataSource::nonMemoizedCumulatedHeightFromIndex(j) + j * horizontalBorderHeight();
  }
};

} // namespace Inference

#endif /* INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_BORDERED_TABLE_VIEW_DATA_SOURCE_H */
