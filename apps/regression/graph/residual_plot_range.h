#ifndef REGRESSION_RESIDUAL_PLOT_RANGE_H
#define REGRESSION_RESIDUAL_PLOT_RANGE_H

#include <apps/shared/memoized_curve_view_range.h>

namespace Regression {

class ResidualPlotRange : public Shared::MemoizedCurveViewRange {
public:
  using Shared::MemoizedCurveViewRange::MemoizedCurveViewRange;
  // Calibrate the range to respect margin and display all data
  void calibrate(double xMin, double xMax, double yMin, double yMax, KDCoordinate height, KDCoordinate bannerHeight);
private:
  constexpr static double k_relativeMargin = 0.1;
  void calibrateXorY(bool isX, double min, double max, KDCoordinate height = -1, KDCoordinate bannerHeight = -1);
};

}

#endif
