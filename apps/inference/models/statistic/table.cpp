#include "table.h"

#include <algorithm>

namespace Inference {

void Table::deleteParametersInColumn(int column) {
  for (int i = 0; i < maxNumberOfRows(); i++) {
    setParameterAtPosition(k_undefinedValue, i, column);
  }
}

bool Table::deleteParameterAtPosition(int row, int column) {
  if (std::isnan(parameterAtPosition(row, column))) {
    // Param is already deleted
    return false;
  }
  setParameterAtPosition(k_undefinedValue, row, column);
  for (int i = 0; i < maxNumberOfColumns(); i++) {
    if (i != column && !std::isnan(parameterAtPosition(row, i))) {
      // There is another non deleted value in this row
      return false;
    }
  }
  return true;
}

Table::Index2D Table::computeDimensions() const {
  Table::Index2D dim = computeInnerDimensions();
  bool displayLastEmptyRow = dim.row < maxNumberOfRows();
  bool displayLastEmptyColumn = dim.col < maxNumberOfColumns();
  Table::Index2D initialDim = initialDimensions();
  return Index2D{.row = std::max(initialDim.row, dim.row + displayLastEmptyRow), .col = std::max(initialDim.col, dim.col + displayLastEmptyColumn)};
}

Table::Index2D Table::computeInnerDimensions() const {
  int maxCol = -1, maxRow = -1;
  for (int row = 0; row < maxNumberOfRows(); row++) {
    for (int col = 0; col < maxNumberOfColumns(); col++) {
      double p = parameterAtPosition(row, col);
      if (!std::isnan(p)) {
        if (row > maxRow) {
          maxRow = row;
        }
        if (col > maxCol) {
          maxCol = col;
        }
      }
    }
  }
  return Index2D{.row = maxRow + 1, .col = maxCol + 1};
}

Table::Index2D Table::indexToIndex2D(int index) const {
  return Index2D{
      .row = index / maxNumberOfColumns(),
      .col = index % maxNumberOfColumns()
    };
}

int Table::index2DToIndex(Index2D indexes) const {
  return index2DToIndex(indexes.row, indexes.col);
}

int Table::index2DToIndex(int row, int column) const {
  return column + row * maxNumberOfColumns();
}

}  // namespace Inference
