#include "inference.h"

namespace Shared {

Inference::Inference() : MemoizedCurveViewRange() {}

void Inference::computeCurveViewRange() {
  // x range
  assert(computeXMin() < computeXMax());
  Poincare::Range1D xRange(computeXMin(), computeXMax());
  xRange.stretchIfTooSmall();
  protectedSetX(xRange);

  // y range
  assert(computeYMin() < computeYMax());
  Poincare::Range1D yRange(computeYMin(), computeYMax());
  if (!yRange.isValid()) {
    yRange = DefaultYRange();
  }
  yRange.stretchIfTooSmall();
  protectedSetY(yRange);
}

bool Inference::authorizedParameterAtIndex(double x, int index) const {
  return std::isfinite(static_cast<float>(x));
}

}  // namespace Shared
