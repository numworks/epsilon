#include "t_statistic.h"

#include <poincare/normal_distribution.h>

namespace Probability {

using namespace Poincare;

float TStatistic::normedDensityFunction(float x) {
  return NormalDistribution::EvaluateAtAbscissa<float>(x, 0, 1);
}

float TStatistic::_tAlpha(float degreesOfFreedom, float alpha) {
  return _tCritical(degreesOfFreedom, alpha);
}

float TStatistic::_pVal(float degreesOfFreedom, float t) {
  switch (m_hypothesisParams.op()) {
    case HypothesisParams::ComparisonOperator::Higher:
      return 1 - NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(t, 0, 1);
    case HypothesisParams::ComparisonOperator::Lower:
      return NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(t, 0, 1);
    case HypothesisParams::ComparisonOperator::Different:
      assert(t > 0);
      return NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(-t / 2, 0, 1) + 1 -
             NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(t / 2, 0, 1);
  }
}

float TStatistic::_tCritical(float degreesOfFreedom, float confidenceLevel) {
  // TODO correct student law
  return NormalDistribution::CumulativeDistributiveInverseForProbability<float>(confidenceLevel, 0,
                                                                                1);
}

}  // namespace Probability
