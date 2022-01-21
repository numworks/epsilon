#include "homogeneity_test.h"
#include <assert.h>
#include <float.h>
#include <probability/models/statistic/interfaces/significance_tests.h>

namespace Probability {

HomogeneityTest::HomogeneityTest() {
  for (int i = 0; i < numberOfStatisticParameters(); i++) {
    m_input[i] = k_undefinedValue;
    m_expectedValues[i] = k_undefinedValue;
  }
}

void HomogeneityTest::setParameterAtPosition(int row, int column, double value) {
  assert(index2DToIndex(row, column) < indexOfThreshold());
  setParameterAtIndex(index2DToIndex(row, column), value);
}

double HomogeneityTest::parameterAtPosition(int row, int column) const {
  return parameterAtIndex(index2DToIndex(row, column));
}

bool HomogeneityTest::authorizedParameterAtIndex(double p, int i) const {
  if (i < numberOfValuePairs() && p < 0.0) {
    // Frequencies should be >= 0
    return false;
  }
  return Chi2Test::authorizedParameterAtIndex(i, p);
}

bool HomogeneityTest::authorizedParameterAtPosition(int row, int column, double p) const {
  return authorizedParameterAtIndex(index2DToIndex(row, column), p);
}

bool HomogeneityTest::deleteParameterAtPosition(int row, int column) {
  if (std::isnan(parameterAtPosition(row, column))) {
    // Param is already deleted
    return false;
  }
  setParameterAtPosition(row, column, k_undefinedValue);
  bool shouldDeleteRow = true;
  for (int i = 0; i < k_maxNumberOfColumns; i++) {
    if (i != column && !std::isnan(parameterAtPosition(row, i))) {
      // There is another non deleted value in this row
      shouldDeleteRow = false;
      break;
    }
  }
  if (shouldDeleteRow) {
    return true;
  }
  bool shouldDeleteCol = true;
  for (int j = 0; j < k_maxNumberOfRows; j++) {
    if (j != row && !std::isnan(parameterAtPosition(j, column))) {
      // There is another non deleted value in this column
      shouldDeleteCol = false;
      break;
    }
  }
  return shouldDeleteCol;
}

void HomogeneityTest::computeTest() {
  Index2D max = computeDimensions();
  m_numberOfResultRows = max.row;
  m_numberOfResultColumns = max.col;
  computeExpectedValues(max);
  m_testCriticalValue = computeChi2();
  m_degreesOfFreedom = computeDegreesOfFreedom(max);
  m_pValue = SignificanceTest::ComputePValue(this);
}

double HomogeneityTest::expectedValueAtLocation(int row, int column) {
  return m_expectedValues[index2DToIndex(row, column)];
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

HomogeneityTest::Index2D HomogeneityTest::computeDimensions() const {
  int maxCol = -1, maxRow = -1;
  for (int row = 0; row < k_maxNumberOfRows; row++) {
    for (int col = 0; col < k_maxNumberOfColumns; col++) {
      double p = parameterAtPosition(row, col);
      if (!std::isnan(p)) {
        if (row >= maxRow) {
          maxRow = row;
        }
        if (col >= maxCol) {
          maxCol = col;
        }
      }
    }
  }
  return Index2D{.row = maxRow + 1, .col = maxCol + 1};
}

int HomogeneityTest::numberOfValuePairs() const {
  Index2D max = computeDimensions();
  return max.row * max.col;
}

HomogeneityTest::Index2D HomogeneityTest::indexToIndex2D(int index) {
  return HomogeneityTest::Index2D{.row = index / k_maxNumberOfColumns,
                                       .col = index % k_maxNumberOfColumns};
}

int HomogeneityTest::index2DToIndex(Index2D indexes) const {
  return index2DToIndex(indexes.row, indexes.col);
}

int HomogeneityTest::index2DToIndex(int row, int column) const {
  return column + row * k_maxNumberOfColumns;
}

HomogeneityTest::Index2D HomogeneityTest::resultsIndexToIndex2D(int resultsIndex) const {
  assert(m_numberOfResultColumns > 0);
  return HomogeneityTest::Index2D{.row = resultsIndex / m_numberOfResultColumns,
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
      double p = parameterAtPosition(row, col);
      m_columnTotals[col] += p;
      m_rowTotals[row] += p;
      m_total += p;
    }
  }
  // Then fill array
  for (int row = 0; row < max.row; row++) {
    for (int col = 0; col < max.col; col++) {
      int index = index2DToIndex(row, col);
      m_expectedValues[index] = m_rowTotals[row] * m_columnTotals[col] / m_total;
    }
  }
}

void HomogeneityTest::recomputeData() {
  // Remove empty rows / columns
  Index2D dimensions = computeDimensions();
  // Start with rows
  int i = 0, j = 0;
  while (i < dimensions.row) {
    bool rowIsEmpty = true;
    for (int col = 0; col < dimensions.col; col++) {
      if (!std::isnan(parameterAtPosition(i, col))) {
        rowIsEmpty = false;
        break;
      }
    }
    if (!rowIsEmpty) {
      if (i != j) {
        // Copy row from i to j
        for (int col = 0; col < dimensions.col; col++) {
          setParameterAtPosition(j, col, parameterAtPosition(i, col));
          setParameterAtPosition(i, col, k_undefinedValue);
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
      if (!std::isnan(parameterAtPosition(row, i))) {
        colIsEmpty = false;
        break;
      }
    }
    if (!colIsEmpty) {
      if (i != j) {
        for (int row = 0; row < dimensions.row; row++) {
          setParameterAtPosition(row, j, parameterAtPosition(row, i));
          setParameterAtPosition(row, i, k_undefinedValue);
        }
      }
      j++;
    }
    i++;
  }
}

bool HomogeneityTest::validateInputs() {
  Index2D max = computeDimensions();
  if (max.col <= 1 || max.row <= 1) {
    return false;
  }
  /* - No value should be undef
   * - Neither a whole row nor a whole column should be null. */
  bool nullRow[k_maxNumberOfRows];
  bool nullColumn[k_maxNumberOfColumns];
  for (int row = 0; row < max.row; row++) {
    // Init nullRow array
    nullRow[row] = true;
    for (int col = 0; col < max.col; col++) {
      if (row == 0) {
        // Init nullColumn array
        nullColumn[col] = true;
      }
      double value = parameterAtPosition(row, col);
      nullRow[row] = nullRow[row] && std::fabs(value) < DBL_MIN;
      nullColumn[col] = nullColumn[col] && std::fabs(value) < DBL_MIN;
      if (std::isnan(value)) {
        return false;
      }
      if (row == max.row - 1 ) {
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
  return true;
}

}  // namespace Probability
