#include "inference.h"

namespace Shared {

Inference::Inference() : MemoizedCurveViewRange() {}

void Inference::computeCurveViewRange() {
  // x range
  protectedSetXRange(computeXMin(), computeXMax());

  // y range
  Poincare::Range1D yRange(computeYMin(), computeYMax());
  if (yRange.isNan()) {
    yRange = DefaultYRange();
  }
  protectedSetYRange(yRange);
}

bool Inference::authorizedParameterAtIndex(double x, int index) const {
  return std::isfinite(static_cast<float>(x));
}

}  // namespace Shared
