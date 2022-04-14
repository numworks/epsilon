#include "inference.h"
#include "probability/distribution/binomial_distribution.h"
#include "statistic/one_mean_t_test.h"
#include "statistic/one_mean_t_interval.h"

namespace Inference {

Inference::Inference() : Shared::MemoizedCurveViewRange() {
}

bool Inference::Initialize(Inference * inference, SubApp subApp) {
  if (inference->subApp() == subApp) {
    return false;
  }
  inference->~Inference();
  Statistic * s = nullptr;
  switch (subApp) {
    case SubApp::Probability:
      new (inference) BinomialDistribution();
      break;
    case SubApp::Test:
      s = new (inference) OneMeanTTest();
      break;
    case SubApp::Interval:
      s = new (inference) OneMeanTInterval();
      break;
    default:
      assert(false);
  }
  if (s) {
    s->initParameters();
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
