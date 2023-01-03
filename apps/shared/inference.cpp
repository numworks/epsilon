#include "inference.h"

namespace Shared {

Inference::Inference() : MemoizedCurveViewRange() {
}

void Inference::stretchRangeIfTooClose(float * min, float * max) const {
  if (*max - *min >= Poincare::Range1D::k_minLength) {
    return;
  }
  *max += Poincare::Range1D::k_minLength;
  *min -= Poincare::Range1D::k_minLength;
  assert(*max - *min >= Poincare::Range1D::k_minLength);
}

void Inference::computeCurveViewRange() {
  float xMin = computeXMin();
  float xMax = computeXMax();
  stretchRangeIfTooClose(&xMin, &xMax);
  protectedSetX(Poincare::Range1D(xMin, xMax));
  protectedSetY(Poincare::Range1D(computeYMin(), computeYMax()));
}

bool Inference::authorizedParameterAtIndex(double x, int index) const {
  return std::isfinite(static_cast<float>(x));
}

}
