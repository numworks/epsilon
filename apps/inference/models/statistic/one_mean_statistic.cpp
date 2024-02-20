#include "one_mean_statistic.h"

namespace Inference {

void OneMeanStatistic::setSeries(int series) {
  if (series < 0 && m_series >= 0) {
    reinitParameters();
  }
  m_series = series;
  syncParametersWithStore();
}

void OneMeanStatistic::setParameterAtPosition(double value, int row,
                                              int column) {
  assert(m_store);
  m_store->set(value, m_series, column, row, false, true);
  syncParametersWithStore();
}

bool OneMeanStatistic::deleteParameterAtPosition(int row, int column) {
  assert(m_store);
  if (std::isnan(parameterAtPosition(row, column))) {
    // Param is already deleted
    return false;
  }
  int numberOfPairs = m_store->numberOfPairsOfSeries(m_series);
  setParameterAtPosition(k_undefinedValue, row, column);
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
