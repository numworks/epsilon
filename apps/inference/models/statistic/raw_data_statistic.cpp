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
  if (hasSeries() && !validateSeries(this)) {
    return false;
  }
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
  set(value, seriesAt(toUint(m_activePageIndex) - 1), relativeColumn(column),
      row, false, true);
}

double RawDataStatistic::parameterAtPosition(int row, int column) const {
  int series = seriesAt(toUint(m_activePageIndex) - 1);
  if (row >= numberOfPairsOfSeries(series)) {
    return NAN;
  }
  return get(series, relativeColumn(column), row);
}

void RawDataStatistic::deleteParametersInColumn(int column) {
  clearColumn(seriesAt(toUint(m_activePageIndex) - 1), relativeColumn(column));
}

bool RawDataStatistic::deleteParameterAtPosition(int row, int column) {
  if (std::isnan(parameterAtPosition(row, column))) {
    // Param is already deleted
    return false;
  }
  int series = seriesAt(toUint(m_activePageIndex) - 1);
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

bool RawDataStatistic::computedParameterAtIndex(int* index, Statistic* stat,
                                                double* value,
                                                Poincare::Layout* message,
                                                I18n::Message* subMessage,
                                                int* precision) {
  if (!hasSeries()) {
    return false;
  }
  if (*index >= stat->numberOfStatisticParameters()) {
    *index -= stat->numberOfStatisticParameters();
    return false;
  }

  *precision = Poincare::Preferences::MediumNumberOfSignificantDigits;

  /* For Z distribution, the computed parameter at index 1 (and 4 in case of
   * TwoMeans) is not the parameter at that index (which is the population
   * standard deviation).*/
  if (stat->distributionType() != DistributionType::Z ||
      *index % OneMean::k_numberOfParams != 1) {
    *value = stat->parameterAtIndex(*index);
    *message = stat->parameterSymbolAtIndex(*index);
    *subMessage = stat->parameterDefinitionAtIndex(*index);
    return true;
  }

  /* Weave sample standard deviation between mean and population. */
  *value =
      sampleStandardDeviation(seriesAt(*index / OneMean::k_numberOfParams));
  Shared::ParameterRepresentation repr;
  if (stat->significanceTestType() == SignificanceTestType::OneMean) {
    repr = OneMean::ParameterRepresentationAtIndex(OneMean::Type::T, *index);
  } else {
    assert(stat->significanceTestType() == SignificanceTestType::TwoMeans);
    repr = TwoMeans::ParameterRepresentationAtIndex(TwoMeans::Type::T, *index);
  }
  *message = repr.m_symbol;
  *subMessage = repr.m_description;

  return true;
}

}  // namespace Inference
