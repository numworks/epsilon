#include "chi2_statistic.h"

#include <assert.h>

#include <cmath>

#include "probability/models/chi2_law.h"

namespace Probability {

Chi2Statistic::Chi2Statistic() : m_degreesOfFreedom(-1) {
  m_hypothesisParams.setOp(HypothesisParams::ComparisonOperator::Higher);  // Always higher
}

float Chi2Statistic::normedDensityFunction(float x) {
  return Chi2Law::EvaluateAtAbscissa(x, m_degreesOfFreedom);
}

float Chi2Statistic::_z() {
  float z = 0;
  int n = numberOfValuePairs();
  for (int i = 0; i < n; i++) {
    z += std::pow(expectedValue(i) - observedValue(i), 2) / expectedValue(i);
  }
  return z;
}

float Chi2Statistic::_zAlpha(float degreesOfFreedom, float significanceLevel) {
  return Chi2Law::CumulativeDistributiveInverseForProbability(1 - significanceLevel,
                                                              degreesOfFreedom);
}

float Chi2Statistic::_pVal(float degreesOfFreedom, float z) {
  return 1 - Chi2Law::CumulativeDistributiveFunctionAtAbscissa<float>(z, degreesOfFreedom);
}

}  // namespace Probability
