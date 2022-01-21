#include "inference.h"
#include "probability/distribution/binomial_distribution.h"
#include "statistic/one_mean_t_test.h"
#include "statistic/one_mean_t_interval.h"

namespace Probability {

Inference::Inference() : Shared::MemoizedCurveViewRange() {
}

void Inference::Initialize(Inference * inference, SubApp subApp) {
  inference->~Inference();
  switch (subApp) {
    case SubApp::Probability:
      new (inference) BinomialDistribution();
      return;
    case SubApp::Test:
      new (inference) OneMeanTTest();
      return;
    case SubApp::Interval:
      new (inference) OneMeanTInterval();
      return;
    default:
      assert(false);
  }
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
