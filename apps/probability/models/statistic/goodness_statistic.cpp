#include "goodness_statistic.h"

namespace Probability {

GoodnessStatistic::GoodnessStatistic() {
  for (int i = 0; i < k_maxNumberOfRows * 2; i++) {
    m_input[i] = k_undefinedValue;
  }
}

void GoodnessStatistic::computeTest() {
  m_degreesOfFreedom = computeDegreesOfFreedom();
  m_testCriticalValue = computeChi2();
  m_zAlpha = computeZAlpha(m_threshold, m_hypothesisParams.op());
  m_pValue = computePValue(m_testCriticalValue, m_hypothesisParams.op());
}

void GoodnessStatistic::recomputeData() {
  // Delete empty rows
  int j = 0;
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    if (!(std::isnan(expectedValue(i)) && std::isnan(observedValue(i)))) {
      if (i != j) {
        setExpectedValue(j, expectedValue(i));
        setObservedValue(j, observedValue(i));
      }
      j++;
    }
  }
  while (j < k_maxNumberOfRows) {
    setExpectedValue(j, k_undefinedValue);
    setObservedValue(j, k_undefinedValue);
    j++;
  }
}

int GoodnessStatistic::computeNumberOfRows() {
  // Compute number of rows based on undefined flag
  int i = k_maxNumberOfRows - 1;
  while (i >= 0 && std::isnan(expectedValue(i)) && std::isnan(observedValue(i))) {
    i--;
  }
  return i + 1;
}

int GoodnessStatistic::numberOfValuePairs() {
  return computeNumberOfRows();
}

float GoodnessStatistic::expectedValue(int index) {
  return paramArray()[2 * index + 1];
}

float GoodnessStatistic::observedValue(int index) {
  return paramArray()[2 * index];
}

void GoodnessStatistic::setExpectedValue(int index, float value) {
  paramArray()[2 * index + 1] = value;
}

void GoodnessStatistic::setObservedValue(int index, float value) {
  paramArray()[2 * index] = value;
}

}  // namespace Probability
