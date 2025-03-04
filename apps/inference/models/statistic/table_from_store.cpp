#include "table_from_store.h"

#include "messages.h"
#include "shared/double_pair_store.h"

namespace Inference {

void TableFromStore::setSeriesAt(Statistic* stat, int pageIndex, int series) {
  assert(pageIndex >= 0 && pageIndex < m_series.size());
  m_series[pageIndex] = series;
  if (!hasSeries(pageIndex) && !stat->areParametersValid()) {
    stat->initParameters();
  }
}

bool TableFromStore::validateInputs(Statistic* stat, int pageIndex) {
  assert(pageIndex >= 0 && pageIndex < numberOfSeries());
  if (hasSeries(pageIndex)) {
    if (!validateSeries(doublePairStore(), pageIndex)) {
      return false;
    }
    computeParametersFromSeries(stat, pageIndex);
  }
  return stat->areParametersValid();
}

bool TableFromStore::authorizedValueAtPosition(double p, int row,
                                               int column) const {
  return const_cast<TableFromStore*>(this)
      ->doublePairStore()
      ->valueValidInColumn(p, column);
}

void TableFromStore::setValueAtPosition(double value, int row, int column) {
  doublePairStore()->set(value, seriesAt(m_activePageIndex),
                         doublePairStore()->relativeColumn(column), row, false,
                         true);
}

double TableFromStore::valueAtPosition(int row, int column) const {
  int series = seriesAt(m_activePageIndex);
  Shared::DoublePairStore* store =
      const_cast<TableFromStore*>(this)->doublePairStore();
  if (row >= store->numberOfPairsOfSeries(series)) {
    return NAN;
  }
  return store->get(series, store->relativeColumn(column), row);
}

bool TableFromStore::deleteValueAtPosition(int row, int column) {
  if (std::isnan(valueAtPosition(row, column))) {
    // Param is already deleted
    return false;
  }
  int series = seriesAt(m_activePageIndex);
  int numberOfPairs = doublePairStore()->numberOfPairsOfSeries(series);
  doublePairStore()->deletePairOfSeriesAtIndex(series, row);
  // DoublePairStore::updateSeries has handled the deletion of empty rows
  return numberOfPairs != doublePairStore()->numberOfPairsOfSeries(series);
}

void TableFromStore::recomputeData() {
  for (int i = 0; i < numberOfSeries(); i++) {
    int seriesAtIndex = m_series[i];
    if (seriesAtIndex >= 0) {
      doublePairStore()->updateSeries(seriesAtIndex);
    }
  }
}

void TableFromStatisticStore::setSeriesAt(Statistic* stat, int pageIndex,
                                          int series) {
  TableFromStore::setSeriesAt(stat, pageIndex, series);
  initDatasetsIfSeries();
}

void TableFromStatisticStore::deleteValuesInColumn(int column) {
  clearColumn(seriesAt(m_activePageIndex),
              doublePairStore()->relativeColumn(column));
}

bool TableFromStatisticStore::computedParameterAtIndex(
    int index, Statistic* stat, double* value, Poincare::Layout* message,
    I18n::Message* subMessage, int* precision) {
  *precision = Poincare::Preferences::MediumNumberOfSignificantDigits;

  constexpr int k_oneMeanNumberOfParams =
      PcrInference::NumberOfParameters(PcrInference::TestType::OneMean);

  /* For Z distribution, the computed parameter at index 1 (and 4 in case of
   * TwoMeans) is not the parameter at that index (which is the population
   * standard deviation).*/
  if (stat->statisticType() != PcrInference::StatisticType::Z ||
      index % k_oneMeanNumberOfParams != PcrInference::Params::OneMean::S) {
    *value = stat->parameterAtIndex(index);
    *message = stat->parameterSymbolAtIndex(index);
    *subMessage = stat->parameterDefinitionAtIndex(index);
    return true;
  }

  /* Weave sample standard deviation between mean and population. */
  *value = sampleStandardDeviation(seriesAt(index / k_oneMeanNumberOfParams));

  PcrInference::Type tType(stat->type().testType,
                           PcrInference::StatisticType::T);
  *message = PcrInference::ParameterLayout(tType, index);
  *subMessage = ParameterDescriptionAtIndex(tType, index);

  return true;
}

}  // namespace Inference
