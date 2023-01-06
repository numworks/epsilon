#include "inference.h"

namespace Shared {

Inference::Inference() : MemoizedCurveViewRange() {
}

void Inference::computeCurveViewRange() {
  Poincare::Range1D xRange(computeXMin(), computeXMax());
  xRange.stretchIfTooSmall();
  protectedSetX(xRange);
  protectedSetY(Poincare::Range1D(computeYMin(), computeYMax()));
}

bool Inference::authorizedParameterAtIndex(double x, int index) const {
  return std::isfinite(static_cast<float>(x));
}

}
