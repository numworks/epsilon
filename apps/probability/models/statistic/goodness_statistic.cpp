#include "goodness_statistic.h"
#include <float.h>

namespace Probability {

GoodnessStatistic::GoodnessStatistic() {
  for (int i = 0; i < k_maxNumberOfRows * k_maxNumberOfCols; i++) {
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
  int lastNonEmptyRow = -1;
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    if (!(std::isnan(expectedValue(i)) && std::isnan(observedValue(i)))) {
      if (i != j) {
        setExpectedValue(j, expectedValue(i));
        setObservedValue(j, observedValue(i));
      }
      j++;
      lastNonEmptyRow = i;
    }
  }
  while (j <= lastNonEmptyRow) {
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

double GoodnessStatistic::paramAtLocation(int row, int column) {
  return paramAtIndex(locationToTableIndex(row, column));
}

void GoodnessStatistic::setParamAtLocation(int row, int column, double p) {
  setParamAtIndex(locationToTableIndex(row, column), p);
}

bool GoodnessStatistic::isValidParamAtLocation(int row, int column, double p) {
  return isValidParamAtIndex(locationToTableIndex(row, column), p);
}

bool GoodnessStatistic::isValidParamAtIndex(int i, double p) {
  if (i % k_maxNumberOfCols == 1 && std::fabs(p) < DBL_MIN) {
    // Expected value should not be null
    return false;
  }
  return Chi2Statistic::isValidParamAtIndex(i, p);
}

bool GoodnessStatistic::deleteParamAtLocation(int row, int column) {
  /* Delete param at location, return true if the deleted param was the last non
   * deleted value of its row. */
  if (std::isnan(paramAtLocation(row, column))) {
    // Param is already deleted
    return false;
  }
  setParamAtLocation(row, column, k_undefinedValue);
  for (size_t i = 0; i < k_maxNumberOfCols; i++) {
    if (i != column && !std::isnan(paramAtLocation(row, i))) {
      // There is another non deleted value in this row
      return false;
    }
  }
  return true;
}

int GoodnessStatistic::numberOfValuePairs() {
  return computeNumberOfRows();
}

int GoodnessStatistic::locationToTableIndex(int row, int column) {
  assert((column >= 0 && column < k_maxNumberOfCols) && (row >= 0 && row < k_maxNumberOfRows));
  return k_maxNumberOfCols * row + column;
}

double GoodnessStatistic::expectedValue(int index) {
  return paramArray()[locationToTableIndex(index, 1)];
}

double GoodnessStatistic::observedValue(int index) {
  return paramArray()[locationToTableIndex(index, 0)];
}

void GoodnessStatistic::setExpectedValue(int index, double value) {
  paramArray()[locationToTableIndex(index, 1)] = value;
}

void GoodnessStatistic::setObservedValue(int index, double value) {
  paramArray()[locationToTableIndex(index, 0)] = value;
}

}  // namespace Probability
