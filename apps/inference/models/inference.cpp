#include "inference.h"
#include "statistic/one_mean_t_test.h"
#include "statistic/one_mean_t_interval.h"

namespace Inference {

Inference::Inference() : Shared::MemoizedCurveViewRange() {
}


void Inference::computeCurveViewRange() {
  protectedSetXMin(computeXMin(), Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, false);
  protectedSetXMax(computeXMax(), Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, true);
  protectedSetYMin(computeYMin(), Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, false);
  protectedSetYMax(computeYMax(), Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, true);
}

bool Inference::authorizedParameterAtIndex(double x, int index) const {
  return std::isfinite(static_cast<float>(x));
}

}
