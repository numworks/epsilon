#include "chi2_statistic.h"

#include <assert.h>

#include <cmath>

#include "probability/models/chi2_law.h"

namespace Probability {

void Chi2Statistic::computeTest() {
  m_degreesOfFreedom = _degreesOfFreedom();
  m_z = 0;
  for (int i = 0; i < numberOfValuePairs(); i++) {
    m_z += std::pow(expectedValue(i) - observedValue(i), 2) / expectedValue(i);
  }
  m_zAlpha = absIfNeeded(_zAlpha(m_degreesOfFreedom, m_threshold));
  m_pValue = _pVal(m_degreesOfFreedom, m_z);
}

float Chi2Statistic::normedDensityFunction(float x) {
  return Chi2Law::EvaluateAtAbscissa(x, m_degreesOfFreedom);
}

float Chi2Statistic::_zAlpha(float degreesOfFreedom, float significanceLevel) {
  return Chi2Law::CumulativeDistributiveInverseForProbability(1 - significanceLevel, degreesOfFreedom);
}

float Chi2Statistic::_pVal(float degreesOfFreedom, float z) {
  return 1 - Chi2Law::CumulativeDistributiveFunctionAtAbscissa<float>(z, degreesOfFreedom);
}

}  // namespace Probability
