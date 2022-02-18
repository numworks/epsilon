#include "goodness_test.h"
#include <float.h>
#include <poincare/print.h>
#include <probability/models/statistic/interfaces/significance_tests.h>

namespace Probability {

GoodnessTest::GoodnessTest() {
  for (int i = 0; i < k_maxNumberOfRows * k_maxNumberOfColumns; i++) {
    m_input[i] = k_undefinedValue;
  }
}

void GoodnessTest::setGraphTitle(char * buffer, size_t bufferSize) const {
  const char * format = I18n::translate(I18n::Message::StatisticGraphControllerTestTitleFormatGoodnessTest);
  Poincare::Print::customPrintf(buffer, bufferSize, format,
      degreeOfFreedom(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits,
      threshold(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits,
      testCriticalValue(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits,
      pValue(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
}

void GoodnessTest::setResultTitle(char * buffer, size_t bufferSize, bool resultIsTopPage) const {
  Poincare::Print::customPrintf(buffer, bufferSize, "df=%*.*ed %s=%*.*ed",
      degreeOfFreedom(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits,
      I18n::translate(I18n::Message::GreekAlpha),
      threshold(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
}

void GoodnessTest::computeTest() {
  m_testCriticalValue = computeChi2();
  m_pValue = SignificanceTest::ComputePValue(this);
}

void GoodnessTest::recomputeData() {
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

int GoodnessTest::computeNumberOfRows() const {
  // Compute number of rows based on undefined flag
  int i = k_maxNumberOfRows - 1;
  while (i >= 0 && std::isnan(expectedValue(i)) && std::isnan(observedValue(i))) {
    i--;
  }
  return i + 1;
}

bool GoodnessTest::validateInputs() {
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

double GoodnessTest::parameterAtPosition(int row, int column) const {
  return parameterAtIndex(locationToTableIndex(row, column));
}

void GoodnessTest::setParameterAtPosition(double p, int row, int column) {
  setParameterAtIndex(p, locationToTableIndex(row, column));
}

bool GoodnessTest::authorizedParameterAtPosition(double p, int row, int column) const {
  return authorizedParameterAtIndex(p, locationToTableIndex(row, column));
}

bool GoodnessTest::authorizedParameterAtIndex(double p, int i) const {
  if (i < numberOfStatisticParameters() && i % k_maxNumberOfColumns == 1 && std::fabs(p) < DBL_MIN) {
    // Expected value should not be null
    return false;
  }
  return Chi2Test::authorizedParameterAtIndex(i, p);
}

// TODO : factorize with HomogeneityTest
bool GoodnessTest::deleteParameterAtPosition(int row, int column) {
  if (std::isnan(parameterAtPosition(row, column))) {
    // Param is already deleted
    return false;
  }
  setParameterAtPosition(k_undefinedValue, row, column);
  for (int i = 0; i < k_maxNumberOfColumns; i++) {
    if (i != column && !std::isnan(parameterAtPosition(row, i))) {
      // There is another non deleted value in this row
      return false;
    }
  }
  return true;
}

int GoodnessTest::numberOfValuePairs() const {
  return computeNumberOfRows();
}

int GoodnessTest::locationToTableIndex(int row, int column) const {
  assert((column >= 0 && column < k_maxNumberOfColumns) && (row >= 0 && row < k_maxNumberOfRows));
  return k_maxNumberOfColumns * row + column;
}

double GoodnessTest::expectedValue(int index) const {
  return m_input[locationToTableIndex(index, 1)];
}

double GoodnessTest::observedValue(int index) const {
  return m_input[locationToTableIndex(index, 0)];
}

void GoodnessTest::setExpectedValue(int index, double value) {
  parametersArray()[locationToTableIndex(index, 1)] = value;
}

void GoodnessTest::setObservedValue(int index, double value) {
  parametersArray()[locationToTableIndex(index, 0)] = value;
}

}  // namespace Probability
