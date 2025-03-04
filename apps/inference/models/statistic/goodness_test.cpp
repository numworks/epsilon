#include "goodness_test.h"

#include <float.h>
#include <inference/statistic/chi_square/results_goodness_table_cell.h>
#include <poincare/print.h>
#include <string.h>

#include "homogeneity_test.h"

namespace Inference {

static_assert(sizeof(GoodnessTest) < sizeof(HomogeneityTest),
              "Make sure this size increase was decided");

GoodnessTest::GoodnessTest() {
  for (int i = 0; i < k_maxNumberOfRows * k_maxNumberOfColumns; i++) {
    m_input[i] = k_undefinedValue;
  }
}

void GoodnessTest::setGraphTitle(char* buffer, size_t bufferSize) const {
  Poincare::Print::CustomPrintf(
      buffer, bufferSize, "df=%*.*ed %s=%*.*ed %s=%*.*ed %s=%*.*ed",
      degreeOfFreedom(), Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::ShortNumberOfSignificantDigits,
      I18n::translate(I18n::Message::GreekAlpha), threshold(),
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::ShortNumberOfSignificantDigits,
      criticalValueSymbol(), testCriticalValue(),
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::ShortNumberOfSignificantDigits,
      I18n::translate(I18n::Message::PValue), pValue(),
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::ShortNumberOfSignificantDigits);
}

void GoodnessTest::setResultTitle(char* buffer, size_t bufferSize,
                                  bool resultIsTopPage) const {
  strlcpy(buffer, I18n::translate(I18n::Message::CalculatedValues), bufferSize);
}

void GoodnessTest::compute() {
  m_testCriticalValue = computeChi2();
  m_pValue =
      PcrInference::ComputePValue(testType(), m_hypothesis.m_alternative,
                                  m_testCriticalValue, degreeOfFreedom());
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

bool GoodnessTest::validateInputs(int pageIndex) {
  if (numberOfValuePairs() <= 1) {
    return false;
  }
  int n = computeInnerDimensions().row;
  for (int row = 0; row < n; row++) {
    if (std::isnan(expectedValue(row)) || std::isnan(observedValue(row))) {
      return false;
    }
  }
  return true;
}

void GoodnessTest::setParameterAtIndex(double p, int i) {
  if (i == indexOfDegreeOfFreedom()) {
    m_degreesOfFreedom = p;
  } else {
    return Statistic::setParameterAtIndex(p, i);
  }
}

bool GoodnessTest::authorizedParameterAtIndex(double p, int i) const {
  if (i < numberOfTestParameters() && i % k_maxNumberOfColumns == 1 &&
      std::fabs(p) < DBL_MIN) {
    // Expected value should not be null
    return false;
  }
  if (i == indexOfDegreeOfFreedom() &&
      (p != std::round(p) || p < 1.0 || p > k_maxDegreeOfFreedom)) {
    return false;
  }
  return Chi2Test::authorizedParameterAtIndex(p, i);
}

double GoodnessTest::valueAtPosition(int row, int column) const {
  if (column == ResultGoodnessContributionsTable::ContributionColumnIndex) {
    // Contribution column
    return computeContribution(row);
  }

  return Chi2Test::valueAtPosition(row, column);
}

int GoodnessTest::numberOfValuePairs() const {
  return computeInnerDimensions().row;
}

double GoodnessTest::expectedValue(int index) const {
  return m_input[index2DToIndex(index, 1)];
}

double GoodnessTest::observedValue(int index) const {
  return m_input[index2DToIndex(index, 0)];
}

void GoodnessTest::setExpectedValue(int index, double value) {
  parametersArray()[index2DToIndex(index, 1)] = value;
}

void GoodnessTest::setObservedValue(int index, double value) {
  parametersArray()[index2DToIndex(index, 0)] = value;
}

}  // namespace Inference
