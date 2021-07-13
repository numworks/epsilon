#include "homogeneity_statistic.h"

#include <assert.h>

namespace Probability {

HomogeneityStatistic::HomogeneityStatistic() :
    m_numberOfInputParams(k_maxNumberOfColumns * k_maxNumberOfRows) {
  for (int i = 0; i < numberOfParameters(); i++) {
    m_input[i] = k_undefinedValue;
  }
}

void HomogeneityStatistic::setParameterAtPosition(int row, int column, float value) {
  setParamAtIndex(index2DToIndex(row, column), value);
}

float HomogeneityStatistic::parameterAtPosition(int row, int column) {
  return paramAtIndex(index2DToIndex(row, column));
}

float HomogeneityStatistic::observedValue(int index) {
  return paramAtIndex(index);
}

float HomogeneityStatistic::expectedValue(int index) {
}

int HomogeneityStatistic::_numberOfInputParams() {
  int maxCol, maxRow;
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
  assert(maxCol > 0 && maxRow > 0);
  return maxRow * maxCol;
}

void HomogeneityStatistic::computeNumberOfParameters() {
  m_numberOfInputParams = _numberOfInputParams();
}

HomogeneityStatistic::Index2D HomogeneityStatistic::indexToTableIndex(int index) {
  return HomogeneityStatistic::Index2D{.row = index / k_maxNumberOfColumns,
                                       .col = index % k_maxNumberOfColumns};
}

int HomogeneityStatistic::index2DToIndex(Index2D indexes) {
  return index2DToIndex(indexes.row, indexes.col);
}

int HomogeneityStatistic::index2DToIndex(int row, int column) {
  return column + row * k_maxNumberOfColumns;
}

}  // namespace Probability
