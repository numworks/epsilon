#include "one_mean_statistic.h"

namespace Inference {

void OneMeanStatistic::setSeries(int series, Statistic* stat) {
  if (series < 0 && !stat->validateInputs()) {
    stat->initParameters();
  }
  m_series = series;
  initDatasetsIfSeries();
}

bool OneMeanStatistic::parametersAreValid(Statistic* stat) {
  syncParametersWithStore();
  for (int i = 0; i < std::size(m_params); i++) {
    if (!stat->authorizedParameterAtIndex(m_params[i], i)) {
      return false;
    }
  }
  return true;
}

void OneMeanStatistic::setParameterAtPosition(double value, int row,
                                              int column) {
  set(value, m_series, column, row, false, true);
}

void OneMeanStatistic::deleteParametersInColumn(int column) {
  clearColumn(m_series, column);
}

bool OneMeanStatistic::deleteParameterAtPosition(int row, int column) {
  if (std::isnan(parameterAtPosition(row, column))) {
    // Param is already deleted
    return false;
  }
  int numberOfPairs = numberOfPairsOfSeries(m_series);
  deletePairOfSeriesAtIndex(m_series, row);
  // DoublePairStore::updateSeries has handled the deletion of empty rows
  return numberOfPairs != numberOfPairsOfSeries(m_series);
}

double OneMeanStatistic::parameterAtPosition(int row, int column) const {
  if (row >= numberOfPairsOfSeries(m_series)) {
    return NAN;
  }
  return get(m_series, column, row);
}

void OneMeanStatistic::syncParametersWithStore() {
  if (m_series < 0) {
    return;
  }
  m_params[OneMean::ParamsOrder::x] = mean(m_series);
  // FIXME Different between Z and T ?
  m_params[OneMean::ParamsOrder::s] = sampleStandardDeviation(m_series);
  m_params[OneMean::ParamsOrder::n] = sumOfOccurrences(m_series);
}

bool OneMeanStatistic::resultOrComputedParameterAtIndex(
    int* index, Statistic* stat, double* value, Poincare::Layout* message,
    I18n::Message* subMessage, int* precision) {
  if (m_series < 0) {
    return false;
  }
  if (*index >= OneMean::k_numberOfParams) {
    *index -= OneMean::k_numberOfParams;
    return false;
  }
  *value = m_params[*index];
  *message = stat->parameterSymbolAtIndex(*index);
  *subMessage = stat->parameterDefinitionAtIndex(*index);
  *precision = Poincare::Preferences::MediumNumberOfSignificantDigits;
  return true;
}

}  // namespace Inference
