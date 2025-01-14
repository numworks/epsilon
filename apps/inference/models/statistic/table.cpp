#include "table.h"

#include <algorithm>

#include "chi2_test.h"
#include "one_mean_interval.h"
#include "one_mean_test.h"
#include "slope_t_interval.h"
#include "slope_t_test.h"
#include "statistic.h"
#include "two_means_interval.h"
#include "two_means_test.h"

namespace Inference {

Table* Table::FromStatistic(Statistic* stat) {
  // FIXME Ugly, inferences models need a new class hierarchy

  bool intervalApp = stat->subApp() == Statistic::SubApp::Interval;
  switch (stat->significanceTestType()) {
    case SignificanceTestType::OneMean:
      if (intervalApp) {
        return static_cast<OneMeanInterval*>(stat);
      } else {
        return static_cast<OneMeanTest*>(stat);
      }
    case SignificanceTestType::TwoMeans:
      if (intervalApp) {
        return static_cast<TwoMeansInterval*>(stat);
      } else {
        return static_cast<TwoMeansTest*>(stat);
      }
    case SignificanceTestType::Slope:
      if (intervalApp) {
        return static_cast<SlopeTInterval*>(stat);
      } else {
        return static_cast<SlopeTTest*>(stat);
      }
    default:
      assert(stat->significanceTestType() == SignificanceTestType::Categorical);
      return static_cast<Chi2Test*>(stat);
  }
}

bool Table::hasSeries(int index) const { return seriesAt(index) >= 0; }

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

void Table::deleteParametersInColumn(int column) {
  int nbOfRows = computeInnerDimensions().row;
  for (int j = nbOfRows - 1; j >= 0; j--) {
    setParameterAtPosition(k_undefinedValue, j, column);
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
  return Index2D{
      .row = std::max(initialDim.row, dim.row + displayLastEmptyRow),
      .col = std::max(initialDim.col, dim.col + displayLastEmptyColumn)};
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
