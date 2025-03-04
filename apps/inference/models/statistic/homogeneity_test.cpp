#include "homogeneity_test.h"

#include <assert.h>
#include <float.h>
#include <poincare/print.h>

namespace Inference {

HomogeneityTest::HomogeneityTest() {
  for (int i = 0; i < numberOfTestParameters(); i++) {
    m_input[i] = k_undefinedValue;
    m_expectedValues[i] = k_undefinedValue;
  }
}

void HomogeneityTest::setGraphTitle(char* buffer, size_t bufferSize) const {
  Poincare::Print::CustomPrintf(
      buffer, bufferSize, "%s=%*.*ed %s=%*.*ed %s=%*.*ed",
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

bool HomogeneityTest::authorizedParameterAtIndex(double p, int i) const {
  if (i < numberOfTestParameters() && p < 0.0) {
    // Frequencies should be >= 0
    return false;
  }
  return Chi2Test::authorizedParameterAtIndex(p, i);
}

bool HomogeneityTest::deleteValueAtPosition(int row, int column) {
  if (Chi2Test::deleteValueAtPosition(row, column)) {
    return true;
  }
  for (int j = 0; j < k_maxNumberOfRows; j++) {
    if (j != row && !std::isnan(valueAtPosition(j, column))) {
      // There is another non deleted value in this column
      return false;
    }
  }
  return true;
}

void HomogeneityTest::compute() {
  Index2D max = computeInnerDimensions();
  m_numberOfResultRows = max.row;
  m_numberOfResultColumns = max.col;
  computeExpectedValues(max);
  m_testCriticalValue = computeChi2();
  m_degreesOfFreedom = computeDegreesOfFreedom(max);
  m_pValue =
      PcrInference::ComputePValue(testType(), m_hypothesis.m_alternative,
                                  m_testCriticalValue, m_degreesOfFreedom);
}

double HomogeneityTest::expectedValueAtLocation(int row, int column) {
  return m_expectedValues[index2DToIndex(row, column)];
}

double HomogeneityTest::contributionAtLocation(int row, int column) {
  double eV = expectedValueAtLocation(row, column);
  double oV = valueAtPosition(row, column);
  return std::pow(eV - oV, 2.) / eV;
}

double HomogeneityTest::observedValue(int resultsIndex) const {
  return parameterAtIndex(resultsIndexToArrayIndex(resultsIndex));
}

double HomogeneityTest::expectedValue(int resultsIndex) const {
  return m_expectedValues[resultsIndexToArrayIndex(resultsIndex)];
}

int HomogeneityTest::computeDegreesOfFreedom(Index2D max) {
  return (max.row - 1) * (max.col - 1);
}

int HomogeneityTest::numberOfValuePairs() const {
  Index2D max = computeInnerDimensions();
  return max.row * max.col;
}

HomogeneityTest::Index2D HomogeneityTest::resultsIndexToIndex2D(
    int resultsIndex) const {
  assert(m_numberOfResultColumns > 0);
  return HomogeneityTest::Index2D{
      .row = resultsIndex / m_numberOfResultColumns,
      .col = resultsIndex % m_numberOfResultColumns};
}

int HomogeneityTest::resultsIndexToArrayIndex(int resultsIndex) const {
  return index2DToIndex(resultsIndexToIndex2D(resultsIndex));
}

void HomogeneityTest::computeExpectedValues(Index2D max) {
  // Compute row, column and overall sums
  m_total = 0;
  for (int row = 0; row < max.row; row++) {
    for (int col = 0; col < max.col; col++) {
      // int index = index2DToIndex(row, col);
      if (row == 0) {
        m_columnTotals[col] = 0;
      }
      if (col == 0) {
        m_rowTotals[row] = 0;
      }
      double p = valueAtPosition(row, col);
      m_columnTotals[col] += p;
      m_rowTotals[row] += p;
      m_total += p;
    }
  }
  // Then fill array
  for (int row = 0; row < max.row; row++) {
    for (int col = 0; col < max.col; col++) {
      int index = index2DToIndex(row, col);
      // Note : Divide before multiplying to avoid some cases of double overflow
      m_expectedValues[index] =
          (m_rowTotals[row] / m_total) * m_columnTotals[col];
    }
  }
}

void HomogeneityTest::recomputeData() {
  // Remove empty rows / columns
  Index2D dimensions = computeInnerDimensions();
  // Start with rows
  int i = 0, j = 0;
  while (i < dimensions.row) {
    bool rowIsEmpty = true;
    for (int col = 0; col < dimensions.col; col++) {
      if (!std::isnan(valueAtPosition(i, col))) {
        rowIsEmpty = false;
        break;
      }
    }
    if (!rowIsEmpty) {
      if (i != j) {
        // Copy row from i to j
        for (int col = 0; col < dimensions.col; col++) {
          setValueAtPosition(valueAtPosition(i, col), j, col);
          setValueAtPosition(k_undefinedValue, i, col);
        }
      }
      j++;
    }
    i++;
  }

  // Columns
  i = 0, j = 0;
  while (i < dimensions.col) {
    bool colIsEmpty = true;
    for (int row = 0; row < dimensions.row; row++) {
      if (!std::isnan(valueAtPosition(row, i))) {
        colIsEmpty = false;
        break;
      }
    }
    if (!colIsEmpty) {
      if (i != j) {
        for (int row = 0; row < dimensions.row; row++) {
          setValueAtPosition(valueAtPosition(row, i), row, j);
          setValueAtPosition(k_undefinedValue, row, i);
        }
      }
      j++;
    }
    i++;
  }
}

bool HomogeneityTest::validateInputs(int pageIndex) {
  Index2D max = computeInnerDimensions();
  if (max.col <= 1 || max.row <= 1) {
    return false;
  }
  /* - No value should be undef
   * - Neither a whole row nor a whole column should be null.
   * - The whole population should not be null. */
  bool nullRow[k_maxNumberOfRows];
  bool nullColumn[k_maxNumberOfColumns];
  double total = 0.;
  for (int col = 0; col < max.col; col++) {
    // Init nullColumn array
    nullColumn[col] = true;
  }
  for (int row = 0; row < max.row; row++) {
    // Init nullRow array
    nullRow[row] = true;
    for (int col = 0; col < max.col; col++) {
      double value = valueAtPosition(row, col);
      total += value;
      nullRow[row] = nullRow[row] && std::fabs(value) < DBL_MIN;
      nullColumn[col] = nullColumn[col] && std::fabs(value) < DBL_MIN;
      if (std::isnan(value)) {
        return false;
      }
      if (row == max.row - 1) {
        // Check column nullity
        if (nullColumn[col]) {
          return false;
        }
      }
    }
    // Check row nullity
    if (nullRow[row]) {
      return false;
    }
  }
  return std::fabs(total) >= DBL_MIN;
}

}  // namespace Inference
