#include "inference.h"

namespace Shared {

Inference::Inference() : MemoizedCurveViewRange() {
}

void Inference::computeCurveViewRange() {
  protectedSetX(Poincare::Range1D(computeXMin(), computeXMax()));
  protectedSetY(Poincare::Range1D(computeYMin(), computeYMax()));
}

bool Inference::authorizedParameterAtIndex(double x, int index) const {
  return std::isfinite(static_cast<float>(x));
}

}
