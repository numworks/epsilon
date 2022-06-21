#include "slope_t_interval.h"

namespace Inference {

bool SlopeTStatistic::deleteParameterAtPosition(int row, int column) {
  if (std::isnan(parameterAtPosition(row, column))) {
    // Param is already deleted
    return false;
  }
  int numberOfPairs = numberOfPairsOfSeries(0);
  setParameterAtPosition(k_undefinedValue, row, column);
  // DoublePairStore::updateSeries has handled the deletion of empty rows
  return numberOfPairs != numberOfPairsOfSeries(0);
}

double SlopeTStatistic::parameterAtPosition(int row, int column) const {
  if (row >= numberOfPairsOfSeries(0)) {
    return NAN;
  }
  return get(0, column, row);
}

bool SlopeTStatistic::authorizedParameterAtIndex(double p, int i) const {
  assert(i == numberOfTableParameters());
  return SignificanceTest::ValidThreshold(p);
}

double SlopeTStatistic::computeStandardError() const {
  double n = doubleCastedNumberOfPairsOfSeries(0);
  double xMean = meanOfColumn(0, 0);
  return std::sqrt((1.0/(n - 2.0)) * leastSquaredSum(0) / squaredOffsettedValueSumOfColumn(0, 0, false, xMean));
}

}  // namespace Inference
