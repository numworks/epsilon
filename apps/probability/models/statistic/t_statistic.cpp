#include "t_statistic.h"

#include <poincare/normal_distribution.h>

namespace Probability {

float TStatistic::_pVal(float degreesOfFreedom, float t) {
  return Poincare::NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(t, 0, 1);
}

float TStatistic::_tCritical(float degreesOfFreedom, float confidenceLevel) {
  // TODO correct student law
  return Poincare::NormalDistribution::CumulativeDistributiveInverseForProbability<float>(confidenceLevel, 0, 1);
}



} // namespace Probability
