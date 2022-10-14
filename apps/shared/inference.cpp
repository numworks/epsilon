#include "inference.h"

namespace Shared {

Inference::Inference() : MemoizedCurveViewRange() {
}

void Inference::computeCurveViewRange() {
  protectedSetXMin(computeXMin(), false);
  protectedSetXMax(computeXMax(), true);
  protectedSetYMin(computeYMin(), false);
  protectedSetYMax(computeYMax(), true);
}

bool Inference::authorizedParameterAtIndex(double x, int index) const {
  return std::isfinite(static_cast<float>(x));
}

}
