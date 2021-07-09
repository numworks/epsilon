#include "chi2_statistic.h"

#include <assert.h>

#include <cmath>

#include "probability/models/chi2_law.h"

namespace Probability {

Chi2Statistic::Chi2Statistic() {
  for (int i = 0; i < k_maxNumberOfParameters; i++) {
    m_input[i] = k_undefinedValue;
  }
}

void Chi2Statistic::computeTest() {
  int n = _numberOfInputRows();
  assert(n > 0);
  m_degreesOfFreedom = n - 1;
  for (int i = 0; i < n; i++) {
    m_z += std::pow(expectedValue(i) - observedValue(i), 2) / expectedValue(i);
  }
  m_zAlpha = absIfNeeded(_zAlpha(m_degreesOfFreedom, m_threshold));
  m_pValue = _pVal(m_degreesOfFreedom, m_z);
}

float Chi2Statistic::normedDensityFunction(float x) {
  return Chi2Law::EvaluateAtAbscissa(x, m_degreesOfFreedom);
}

float Chi2Statistic::expectedValue(int index) {
  return paramArray()[2 * index + 1];
}

float Chi2Statistic::observedValue(int index) {
  return paramArray()[2 * index];
}

int Chi2Statistic::_numberOfInputRows() {
    // Compute number of rows based on undefined flag
  int i = k_maxNumberOfParameters / 2 - 1;
  while (i >= 0 && std::isnan(expectedValue(i)) && std::isnan(observedValue(i))) {
    i--;
  }
  return i + 1;
}

float Chi2Statistic::_zAlpha(float degreesOfFreedom, float significanceLevel) {
  return Chi2Law::CumulativeDistributiveInverseForProbability(1 - significanceLevel, degreesOfFreedom);
}

float Chi2Statistic::_pVal(float degreesOfFreedom, float z) {
  return 1 - Chi2Law::CumulativeDistributiveFunctionAtAbscissa<float>(z, degreesOfFreedom);
}

}  // namespace Probability
