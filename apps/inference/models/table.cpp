#include "table.h"

#include <algorithm>

#include "one_mean_interval.h"
#include "one_mean_test.h"
#include "slope_t_statistic.h"
#include "statistic.h"
#include "two_means_interval.h"
#include "two_means_test.h"

namespace Inference {

bool Table::hasSeries(int pageIndex) const { return seriesAt(pageIndex) >= 0; }

bool Table::hasAllSeries() const {
  for (int i = 0; i < numberOfSeries(); i++) {
    if (!hasSeries(i)) {
      return false;
    }
  }
  return true;
}

void Table::unsetSeries(Statistic* stat) {
  for (int i = 0; i < numberOfSeries(); i++) {
    setSeriesAt(stat, i, -1);
  }
}

void Table::deleteValuesInColumn(int column) {
  int nbOfRows = computeInnerDimensions().row;
  for (int j = nbOfRows - 1; j >= 0; j--) {
    setValueAtPosition(k_undefinedValue, j, column);
  }
}

bool Table::deleteValueAtPosition(int row, int column) {
  if (std::isnan(valueAtPosition(row, column))) {
    // Value is already deleted
    return false;
  }
  setValueAtPosition(k_undefinedValue, row, column);
  for (int i = 0; i < maxNumberOfColumns(); i++) {
    if (i != column && !std::isnan(valueAtPosition(row, i))) {
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
  return Index2D{
      .row = std::max(initialDim.row, dim.row + displayLastEmptyRow),
      .col = std::max(initialDim.col, dim.col + displayLastEmptyColumn)};
}

Table::Index2D Table::computeInnerDimensions() const {
  int maxCol = -1, maxRow = -1;
  for (int row = 0; row < maxNumberOfRows(); row++) {
    for (int col = 0; col < maxNumberOfColumns(); col++) {
      double p = valueAtPosition(row, col);
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
  return Index2D{.row = index / maxNumberOfColumns(),
                 .col = index % maxNumberOfColumns()};
}

int Table::index2DToIndex(Index2D indexes) const {
  return index2DToIndex(indexes.row, indexes.col);
}

int Table::index2DToIndex(int row, int column) const {
  return column + row * maxNumberOfColumns();
}

// TODO: const Shared::DoublePairStore*
bool Table::validateSeries(Shared::DoublePairStore* doublePairStore,
                           int index) const {
  assert(index >= 0 && index < numberOfSeries());
  int series = seriesAt(index);
  return doublePairStore->seriesIsValid(series) &&
         doublePairStore->numberOfPairsOfSeries(series) > 2;
}

}  // namespace Inference
