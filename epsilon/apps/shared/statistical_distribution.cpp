#include "statistical_distribution.h"

namespace Shared {

StatisticalDistribution::StatisticalDistribution() : MemoizedCurveViewRange() {}

void StatisticalDistribution::computeCurveViewRange() {
  // x range
  Poincare::Range1D<float> xRange(computeXMin(), computeXMax());
  if (xRange.isNan()) {
    xRange = DefaultXRange();
  }
  protectedSetXRange(xRange);

  // y range
  Poincare::Range1D<float> yRange(computeYMin(), computeYMax());
  if (yRange.isNan()) {
    yRange = DefaultYRange();
  }
  protectedSetYRange(yRange);
}

bool StatisticalDistribution::authorizedParameterAtIndex(double x,
                                                         int index) const {
  return std::isfinite(static_cast<float>(x));
}

}  // namespace Shared
