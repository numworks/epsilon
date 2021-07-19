#include "goodness_statistic.h"

namespace Probability {

GoodnessStatistic::GoodnessStatistic() {
  for (int i = 0; i < k_maxNumberOfRows * 2; i++) {
    m_input[i] = k_undefinedValue;
  }
}

void GoodnessStatistic::computeTest() {
  m_degreesOfFreedom = _degreesOfFreedom();
  m_z = _z();
  m_zAlpha = absIfNeeded(_zAlpha(m_degreesOfFreedom, m_threshold));
  m_pValue = _pVal(m_degreesOfFreedom, m_z);
}

int GoodnessStatistic::numberOfValuePairs() {
  // Compute number of rows based on undefined flag
  int i = k_maxNumberOfRows - 1;
  while (i >= 0 && std::isnan(expectedValue(i)) && std::isnan(observedValue(i))) {
    i--;
  }
  return i + 1;
}

float GoodnessStatistic::expectedValue(int index) {
  return paramArray()[2 * index + 1];
}

float GoodnessStatistic::observedValue(int index) {
  return paramArray()[2 * index];
}

}  // namespace Probability
