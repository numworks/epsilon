#include "homogeneity_statistic.h"

#include <assert.h>

namespace Probability {

HomogeneityStatistic::HomogeneityStatistic() {
  for (int i = 0; i < numberOfStatisticParameters(); i++) {
    m_input[i] = k_undefinedValue;
    m_expectedValues[i] = k_undefinedValue;
  }
}

void HomogeneityStatistic::setParameterAtPosition(int row, int column, float value) {
  setParamAtIndex(index2DToIndex(row, column), value);
}

float HomogeneityStatistic::parameterAtPosition(int row, int column) {
  return paramAtIndex(index2DToIndex(row, column));
}

void HomogeneityStatistic::computeTest() {
  Index2D max = computeDimensions();
  m_numberOfResultRows = max.row;
  m_numberOfResultColumns = max.col;
  computeExpectedValues(max);
  m_testCriticalValue = computeChi2();
  m_degreesOfFreedom = computeDegreesOfFreedom(max);
  m_zAlpha = computeZAlpha(m_threshold, m_hypothesisParams.op());
  m_pValue = computePValue(m_testCriticalValue, m_hypothesisParams.op());
}

float HomogeneityStatistic::expectedValueAtLocation(int row, int column) {
  return m_expectedValues[index2DToIndex(row, column)];
}

float HomogeneityStatistic::observedValue(int resultsIndex) {
  return paramAtIndex(resultsIndexToArrayIndex(resultsIndex));
}

float HomogeneityStatistic::expectedValue(int resultsIndex) {
  return m_expectedValues[resultsIndexToArrayIndex(resultsIndex)];
}

int HomogeneityStatistic::computeDegreesOfFreedom(Index2D max) {
  return (max.row - 1) * (max.col - 1);
}

HomogeneityStatistic::Index2D HomogeneityStatistic::computeDimensions() {
  int maxCol = -1, maxRow = -1;
  for (int row = 0; row < k_maxNumberOfRows; row++) {
    for (int col = 0; col < k_maxNumberOfColumns; col++) {
      float p = parameterAtPosition(row, col);
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

int HomogeneityStatistic::numberOfValuePairs() {
  Index2D max = computeDimensions();
  return max.row * max.col;
}

HomogeneityStatistic::Index2D HomogeneityStatistic::indexToIndex2D(int index) {
  return HomogeneityStatistic::Index2D{.row = index / k_maxNumberOfColumns,
                                       .col = index % k_maxNumberOfColumns};
}

int HomogeneityStatistic::index2DToIndex(Index2D indexes) {
  return index2DToIndex(indexes.row, indexes.col);
}

int HomogeneityStatistic::index2DToIndex(int row, int column) {
  return column + row * k_maxNumberOfColumns;
}

HomogeneityStatistic::Index2D HomogeneityStatistic::resultsIndexToIndex2D(int resultsIndex) {
  assert(m_numberOfResultColumns > 0);
  return HomogeneityStatistic::Index2D{.row = resultsIndex / m_numberOfResultColumns,
                                       .col = resultsIndex % m_numberOfResultColumns};
}

int HomogeneityStatistic::resultsIndexToArrayIndex(int resultsIndex) {
  return index2DToIndex(resultsIndexToIndex2D(resultsIndex));
}

void HomogeneityStatistic::computeExpectedValues(Index2D max) {
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
      float p = parameterAtPosition(row, col);
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

void HomogeneityStatistic::recomputeData() {
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

bool HomogeneityStatistic::validateInputs() {
  Index2D max = computeDimensions();
  if (max.col <= 1 || max.row <= 1) {
    return false;
  }
  for (int row = 0; row < max.row; row++) {
    for (int col = 0; col < max.col; col++) {
      if (std::isnan(parameterAtPosition(row, col))) {
        return false;
      }
    }
  }
  return true;
}

}  // namespace Probability
