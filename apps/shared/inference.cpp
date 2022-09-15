#include "inference.h"

namespace Shared {

Inference::Inference() : MemoizedCurveViewRange() {
}

void Inference::computeCurveViewRange() {
  protectedSetXMin(computeXMin(), Range1D::k_lowerMaxFloat, Range1D::k_upperMaxFloat, false);
  protectedSetXMax(computeXMax(), Range1D::k_lowerMaxFloat, Range1D::k_upperMaxFloat, true);
  protectedSetYMin(computeYMin(), Range1D::k_lowerMaxFloat, Range1D::k_upperMaxFloat, false);
  protectedSetYMax(computeYMax(), Range1D::k_lowerMaxFloat, Range1D::k_upperMaxFloat, true);
}

bool Inference::authorizedParameterAtIndex(double x, int index) const {
  return std::isfinite(static_cast<float>(x));
}

}
