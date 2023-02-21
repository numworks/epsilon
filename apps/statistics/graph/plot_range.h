#ifndef STATISTICS_PLOT_RANGE_H
#define STATISTICS_PLOT_RANGE_H

#include <apps/shared/memoized_curve_view_range.h>

namespace Statistics {

class PlotRange : public Shared::MemoizedCurveViewRange {
 public:
  // Calibrate the curveView range to respect margins and display all data
  void calibrate(KDCoordinate width, KDCoordinate height,
                 KDCoordinate horizontalMargin, KDCoordinate bottomMargin,
                 KDCoordinate topMargin, float seriesXMin, float seriesXMax,
                 float yMin, float yMax);
};

}  // namespace Statistics

#endif
