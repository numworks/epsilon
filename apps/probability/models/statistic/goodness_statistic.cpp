#include "goodness_statistic.h"
#include <float.h>

namespace Probability {

GoodnessStatistic::GoodnessStatistic() {
  for (int i = 0; i < k_maxNumberOfRows * k_maxNumberOfColumns; i++) {
    m_input[i] = k_undefinedValue;
  }
}

void GoodnessStatistic::computeTest() {
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
  if (numberOfValuePairs() <= 1) {
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

double GoodnessStatistic::parameterAtPosition(int row, int column) {
  return paramAtIndex(locationToTableIndex(row, column));
}

void GoodnessStatistic::setParameterAtPosition(int row, int column, double p) {
  setParamAtIndex(locationToTableIndex(row, column), p);
}

bool GoodnessStatistic::isValidParameterAtPosition(int row, int column, double p) {
  return isValidParamAtIndex(locationToTableIndex(row, column), p);
}

bool GoodnessStatistic::isValidParamAtIndex(int i, double p) {
  if (i % k_maxNumberOfColumns == 1 && std::fabs(p) < DBL_MIN) {
    // Expected value should not be null
    return false;
  }
  return Chi2Statistic::isValidParamAtIndex(i, p);
}

// TODO : factorize with HomogeneityStatistic
bool GoodnessStatistic::deleteParameterAtPosition(int row, int column) {
  if (std::isnan(parameterAtPosition(row, column))) {
    // Param is already deleted
    return false;
  }
  setParameterAtPosition(row, column, k_undefinedValue);
  for (int i = 0; i < k_maxNumberOfColumns; i++) {
    if (i != column && !std::isnan(parameterAtPosition(row, i))) {
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
  assert((column >= 0 && column < k_maxNumberOfColumns) && (row >= 0 && row < k_maxNumberOfRows));
  return k_maxNumberOfColumns * row + column;
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
