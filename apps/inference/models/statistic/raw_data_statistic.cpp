#include "raw_data_statistic.h"

namespace Inference {

void RawDataStatistic::setSeriesAt(Statistic* stat, int index, int series) {
  m_series[index] = series;
  if (!hasSeries() && !stat->validateInputs()) {
    stat->initParameters();
  }
  initDatasetsIfSeries();
}

bool RawDataStatistic::parametersAreValid(Statistic* stat) {
  syncParametersWithStore(stat);
  for (int i = 0; i < stat->numberOfParameters(); i++) {
    if (!stat->authorizedParameterAtIndex(stat->parameterAtIndex(i), i)) {
      return false;
    }
  }
  return true;
}

void RawDataStatistic::setParameterAtPosition(double value, int row,
                                              int column) {
  set(value, seriesAtColumn(column), relativeColumn(column), row, false, true);
}

double RawDataStatistic::parameterAtPosition(int row, int column) const {
  int series = seriesAtColumn(column);
  if (row >= numberOfPairsOfSeries(series)) {
    return NAN;
  }
  return get(series, relativeColumn(column), row);
}

void RawDataStatistic::deleteParametersInColumn(int column) {
  clearColumn(seriesAtColumn(column), relativeColumn(column));
}

bool RawDataStatistic::deleteParameterAtPosition(int row, int column) {
  if (std::isnan(parameterAtPosition(row, column))) {
    // Param is already deleted
    return false;
  }
  int series = seriesAtColumn(column);
  int numberOfPairs = numberOfPairsOfSeries(series);
  deletePairOfSeriesAtIndex(series, row);
  // DoublePairStore::updateSeries has handled the deletion of empty rows
  return numberOfPairs != numberOfPairsOfSeries(series);
}

void RawDataStatistic::recomputeData() {
  for (int i = 0; i < numberOfSeries(); i++) {
    updateSeries(m_series[i]);
  }
}

bool RawDataStatistic::resultOrComputedParameterAtIndex(
    int* index, Statistic* stat, double* value, Poincare::Layout* message,
    I18n::Message* subMessage, int* precision) {
  if (!hasSeries()) {
    return false;
  }
  if (*index >= stat->numberOfStatisticParameters()) {
    *index -= stat->numberOfStatisticParameters();
    return false;
  }
  *value = stat->parameterAtIndex(*index);
  *message = stat->parameterSymbolAtIndex(*index);
  *subMessage = stat->parameterDefinitionAtIndex(*index);
  *precision = Poincare::Preferences::MediumNumberOfSignificantDigits;
  return true;
}

}  // namespace Inference