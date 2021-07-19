#include "homogeneity_statistic.h"

#include <assert.h>

namespace Probability {

HomogeneityStatistic::HomogeneityStatistic() {
  for (int i = 0; i < numberOfParameters(); i++) {
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
  Index2D max = numberOfInputParams();
  m_numberOfResultRows = max.row;
  m_numberOfResultColumns = max.col;
  computeExpectedValues();
  m_z = _z();
  m_degreesOfFreedom = _degreesOfFreedom(max);
  m_zAlpha = absIfNeeded(_zAlpha(m_degreesOfFreedom, m_threshold));
  m_pValue = _pVal(m_degreesOfFreedom, m_z);
}

int HomogeneityStatistic::expectedValueAtLocation(int row, int column) {
  return m_expectedValues[index2DToIndex(row, column)];
}

float HomogeneityStatistic::observedValue(int index) {
  return paramAtIndex(index);
}

float HomogeneityStatistic::expectedValue(int index) {
  return m_expectedValues[index];
}

int HomogeneityStatistic::_degreesOfFreedom(Index2D max) {
  return (max.row - 1) * (max.col - 1);
}

HomogeneityStatistic::Index2D HomogeneityStatistic::numberOfInputParams() {
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
  assert(maxCol >= 0 && maxRow >= 0);
  return Index2D{.row = maxRow + 1, .col = maxCol + 1};
}

int HomogeneityStatistic::numberOfValuePairs() {
  Index2D max = numberOfInputParams();
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

void HomogeneityStatistic::computeExpectedValues() {
  // Compute row, column and overall sums
  m_total = 0;
  Index2D max = numberOfInputParams();
  for (int row = 0; row < max.row; row++) {
    for (int col = 0; col < max.col; col++) {
      float p = parameterAtPosition(row, col);
      m_columnTotals[index2DToIndex(row, col)] += p;
      m_rowTotals[index2DToIndex(row, col)] += p;
      m_total += p;
    }
  }
  // Then fill array
  for (int row = 0; row < max.row; row++) {
    for (int col = 0; col < max.col; col++) {
      int index = index2DToIndex(row, col);
      m_expectedValues[index] = m_rowTotals[index] * m_columnTotals[index] / m_total;
    }
  }
}

}  // namespace Probability
