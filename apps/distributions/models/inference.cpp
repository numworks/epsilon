#include "inference.h"
#include "probability/distribution/binomial_distribution.h"

namespace Distributions {

Inference::Inference() : Shared::MemoizedCurveViewRange() {
}

bool Inference::Initialize(Inference * inference, SubApp subApp) {
  if (inference->subApp() == subApp) {
    return false;
  }
  inference->~Inference();
  switch (subApp) {
    case SubApp::Probability:
      new (inference) BinomialDistribution();
      break;
    default:
      assert(false);
  }
  return true;
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
