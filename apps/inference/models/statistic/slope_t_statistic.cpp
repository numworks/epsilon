#include <omg/round.h>

#include "slope_t_interval.h"

namespace Inference {

bool SlopeTStatistic::deleteParameterAtPosition(int row, int column) {
  if (std::isnan(parameterAtPosition(row, column))) {
    // Param is already deleted
    return false;
  }
  int numberOfPairs = numberOfPairsOfSeries(m_series);
  setParameterAtPosition(k_undefinedValue, row, column);
  // DoublePairStore::updateSeries has handled the deletion of empty rows
  return numberOfPairs != numberOfPairsOfSeries(m_series);
}

double SlopeTStatistic::parameterAtPosition(int row, int column) const {
  if (row >= numberOfPairsOfSeries(m_series)) {
    return NAN;
  }
  return get(m_series, column, row);
}

bool SlopeTStatistic::authorizedParameterAtIndex(double p, int i) const {
  assert(i == numberOfTableParameters());
  return SignificanceTest::ValidThreshold(p);
}

double SlopeTStatistic::computeStandardError() const {
  double n = doubleCastedNumberOfPairsOfSeries(m_series);
  double xMean = meanOfColumn(m_series, 0);
  double SE = std::sqrt((1.0 / (n - 2.0)) * leastSquaredSum(m_series) /
                        squaredOffsettedValueSumOfColumn(m_series, 0, xMean));
  return OMG::LaxToZero(SE);
}

}  // namespace Inference
