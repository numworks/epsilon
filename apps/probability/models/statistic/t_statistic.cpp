#include "t_statistic.h"

#include <poincare/normal_distribution.h>

namespace Probability {

float TStatistic::normedDensityFunction(float x) {
  return Poincare::NormalDistribution::EvaluateAtAbscissa<float>(x, 0, 1);
}

float TStatistic::_tAlpha(float degreesOfFreedom, float alpha) {
  return _tCritical(degreesOfFreedom, alpha);
}

float TStatistic::_pVal(float degreesOfFreedom, float t) {
  return Poincare::NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(t, 0, 1);
}

float TStatistic::_tCritical(float degreesOfFreedom, float confidenceLevel) {
  // TODO correct student law
  return Poincare::NormalDistribution::CumulativeDistributiveInverseForProbability<float>(confidenceLevel, 0, 1);
}

} // namespace Probability
