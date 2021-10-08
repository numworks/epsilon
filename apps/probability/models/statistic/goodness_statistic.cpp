#include "goodness_statistic.h"
#include <float.h>

namespace Probability {

GoodnessStatistic::GoodnessStatistic() {
  for (int i = 0; i < k_maxNumberOfRows * 2; i++) {
    m_input[i] = k_undefinedValue;
  }
}

void GoodnessStatistic::computeTest() {
  m_degreesOfFreedom = computeDegreesOfFreedom();
  m_testCriticalValue = computeChi2();
  m_pValue = computePValue(m_testCriticalValue, m_hypothesisParams.comparisonOperator());
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

bool GoodnessStatistic::validateInputs() {
  if (computeDegreesOfFreedom() <= 0) {
    return false;
  }
  int n = computeNumberOfRows();
  for (int row = 0; row < n; row++) {
    if (std::isnan(expectedValue(row)) || std::isnan(observedValue(row))) {
      return false;
    }
  }
  return true;
}

float GoodnessStatistic::paramAtLocation(int row, int column) {
  return paramAtIndex(locationToTableIndex(row, column));
}

void GoodnessStatistic::setParamAtLocation(int row, int column, float p) {
  setParamAtIndex(locationToTableIndex(row, column), p);
}

bool GoodnessStatistic::isValidParamAtLocation(int row, int column, float p) {
  return isValidParamAtIndex(locationToTableIndex(row, column), p);
}

bool GoodnessStatistic::isValidParamAtIndex(int i, float p) {
  if (i % 2 == 1 && std::fabs(p) < FLT_MIN) {
    // Expected value should not be null
    return false;
  }
  return Chi2Statistic::isValidParamAtIndex(i, p);
}

int GoodnessStatistic::numberOfValuePairs() {
  return computeNumberOfRows();
}

int GoodnessStatistic::locationToTableIndex(int row, int column) {
  assert((column == 0 || column == 1) && (row >= 0 && row < k_maxNumberOfRows));
  return 2 * row + column;
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
