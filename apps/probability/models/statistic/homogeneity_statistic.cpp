#include "homogeneity_statistic.h"

#include <assert.h>

namespace Probability {

HomogeneityStatistic::HomogeneityStatistic() {
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
  // res = sum of row * sum of column / total
  // TODO can be super memoized
  Index2D pos = indexToTableIndex(index);
  Index2D max = _numberOfInputParams();
  float total = 0, rowTotal = 0, colTotal = 0;
  for (int row = 0; row < max.row; row++) {
    for (int col = 0; col < max.col; col++) {
      float p = parameterAtPosition(row, col);
      if (col == pos.col) {
        colTotal += p;
      }
      if (row == pos.row) {
        rowTotal += p;
      }
      total += p;
    }
  }
  return rowTotal * colTotal / total;
}

int HomogeneityStatistic::_degreesOfFreedom() {
  Index2D max = _numberOfInputParams();
  return (max.row - 1) * (max.col - 1);
}

HomogeneityStatistic::Index2D HomogeneityStatistic::_numberOfInputParams() {
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
  Index2D max = _numberOfInputParams();
  return max.row * max.col;
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
