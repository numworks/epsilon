#include "one_mean_statistic.h"

namespace Inference {

void OneMeanStatistic::setSeries(int series, Statistic* stat) {
  if (series < 0 && !stat->validateInputs()) {
    stat->initParameters();
  }
  m_series = series;
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
  assert(m_store);
  m_store->set(value, m_series, column, row, false, true);
}

void OneMeanStatistic::deleteParametersInColumn(int column) {
  assert(m_store);
  m_store->clearColumn(m_series, column);
}

bool OneMeanStatistic::deleteParameterAtPosition(int row, int column) {
  assert(m_store);
  if (std::isnan(parameterAtPosition(row, column))) {
    // Param is already deleted
    return false;
  }
  int numberOfPairs = m_store->numberOfPairsOfSeries(m_series);
  m_store->deletePairOfSeriesAtIndex(m_series, row);
  // DoublePairStore::updateSeries has handled the deletion of empty rows
  return numberOfPairs != m_store->numberOfPairsOfSeries(m_series);
}

double OneMeanStatistic::parameterAtPosition(int row, int column) const {
  assert(m_store);
  if (row >= m_store->numberOfPairsOfSeries(m_series)) {
    return NAN;
  }
  return m_store->get(m_series, column, row);
}

void OneMeanStatistic::syncParametersWithStore() {
  if (m_series < 0) {
    return;
  }
  assert(m_store);
  m_params[OneMean::ParamsOrder::x] = m_store->mean(m_series);
  // FIXME Different between Z and T ?
  m_params[OneMean::ParamsOrder::s] =
      m_store->sampleStandardDeviation(m_series);
  m_params[OneMean::ParamsOrder::n] = m_store->sumOfOccurrences(m_series);
}

}  // namespace Inference
