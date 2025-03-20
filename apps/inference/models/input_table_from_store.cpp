#include "input_table_from_store.h"

#include "messages.h"
#include "shared/double_pair_store.h"

namespace Inference {

void InputTableFromStore::setSeriesAt(InferenceModel* inference, int pageIndex,
                                      int series) {
  assert(pageIndex >= 0 && pageIndex < m_series.size());
  m_series[pageIndex] = series;
  if (!hasSeries(pageIndex) && !inference->areParametersValid()) {
    inference->initParameters();
  }
}

bool InputTableFromStore::validateInputs(InferenceModel* inference,
                                         int pageIndex) {
  assert(pageIndex >= 0 && pageIndex < numberOfSeries());
  if (hasSeries(pageIndex)) {
    if (!validateSeries(doublePairStore(), pageIndex)) {
      return false;
    }
    computeParametersFromSeries(inference, pageIndex);
  }
  return inference->areParametersValid();
}

bool InputTableFromStore::authorizedValueAtPosition(double p, int row,
                                                    int column) const {
  return const_cast<InputTableFromStore*>(this)
      ->doublePairStore()
      ->valueValidInColumn(p, column);
}

void InputTableFromStore::setValueAtPosition(double value, int row,
                                             int column) {
  doublePairStore()->set(value, seriesAt(m_activePageIndex),
                         doublePairStore()->relativeColumn(column), row, false,
                         true);
}

double InputTableFromStore::valueAtPosition(int row, int column) const {
  int series = seriesAt(m_activePageIndex);
  Shared::DoublePairStore* store =
      const_cast<InputTableFromStore*>(this)->doublePairStore();
  if (row >= store->numberOfPairsOfSeries(series)) {
    return NAN;
  }
  return store->get(series, store->relativeColumn(column), row);
}

bool InputTableFromStore::deleteValueAtPosition(int row, int column) {
  int series = seriesAt(m_activePageIndex);
  int col = doublePairStore()->relativeColumn(column);
  if (row >= doublePairStore()->lengthOfColumn(series, col)) {
    return false;
  }
  return doublePairStore()->deleteValueAtIndex(
      series, doublePairStore()->relativeColumn(column), row);
}

void InputTableFromStore::recomputeData() {
  for (int i = 0; i < numberOfSeries(); i++) {
    int seriesAtIndex = m_series[i];
    if (seriesAtIndex >= 0) {
      doublePairStore()->updateSeries(seriesAtIndex);
    }
  }
}

void InputTableFromStatisticStore::setSeriesAt(InferenceModel* inference,
                                               int pageIndex, int series) {
  InputTableFromStore::setSeriesAt(inference, pageIndex, series);
  initDatasetsIfSeries();
}

void InputTableFromStatisticStore::deleteValuesInColumn(int column) {
  clearColumn(seriesAt(m_activePageIndex),
              doublePairStore()->relativeColumn(column));
}

bool InputTableFromStatisticStore::computedParameterAtIndex(
    int index, InferenceModel* inference, double* value,
    Poincare::Layout* message, I18n::Message* subMessage, int* precision) {
  *precision = Poincare::Preferences::MediumNumberOfSignificantDigits;

  constexpr int k_oneMeanNumberOfParams =
      Poincare::Inference::NumberOfParameters(TestType::OneMean);

  /* For Z distribution, the computed parameter at index 1 (and 4 in case of
   * TwoMeans) is not the parameter at that index (which is the population
   * standard deviation).*/
  if (inference->statisticType() != StatisticType::Z ||
      index % k_oneMeanNumberOfParams != Params::OneMean::S) {
    *value = inference->parameterAtIndex(index);
    *message = inference->parameterSymbolAtIndex(index);
    *subMessage = inference->parameterDefinitionAtIndex(index);
    return true;
  }

  /* Weave sample standard deviation between mean and population. */
  *value = sampleStandardDeviation(seriesAt(index / k_oneMeanNumberOfParams));

  Poincare::Inference::Type tType(inference->type().testType, StatisticType::T);
  *message = Poincare::Inference::ParameterLayout(tType, index);
  *subMessage = ParameterDescriptionAtIndex(tType, index);

  return true;
}

void InputTableFromRegressionStore::deleteValuesInColumn(int column) {
  deleteColumn(seriesAt(m_activePageIndex),
               doublePairStore()->relativeColumn(column));
}

}  // namespace Inference
