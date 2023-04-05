#include "linear_regression_store.h"

#include <assert.h>
#include <float.h>
#include <poincare/preferences.h>
#include <string.h>

#include <algorithm>
#include <cmath>

namespace Shared {

/* Linear model helper */
double LinearModelHelper::Slope(double covariance, double variance) {
  return covariance / variance;
}

double LinearModelHelper::YIntercept(double meanOfY, double meanOfX,
                                     double slope) {
  return meanOfY - slope * meanOfX;
}

/* Linear Regression Store */

LinearRegressionStore::LinearRegressionStore(
    GlobalContext* context, DoublePairStorePreferences* preferences)
    : DoublePairStore(context, preferences) {}

/* Calculations */

double LinearRegressionStore::doubleCastedNumberOfPairsOfSeries(
    int series) const {
  return DoublePairStore::numberOfPairsOfSeries(series);
}

double LinearRegressionStore::squaredOffsettedValueSumOfColumn(
    int series, int i, double offset, CalculationOptions options) const {
  return createDatasetFromColumn(series, i, options)
      .offsettedSquaredSum(offset);
}

double LinearRegressionStore::squaredValueSumOfColumn(
    int series, int i, CalculationOptions options) const {
  return squaredOffsettedValueSumOfColumn(series, i, 0.0, options);
}

double LinearRegressionStore::leastSquaredSum(int series) const {
  Poincare::StatisticsDataset<double> xDataset =
      createDatasetFromColumn(series, 0);
  Poincare::StatisticsDataset<double> yDataset =
      createDatasetFromColumn(series, 1);
  // Find the linear regression
  double a = yIntercept(series);
  double b = slope(series);
  return yDataset.squaredSumOffsettedByLinearTransformationOfDataset(xDataset,
                                                                     a, b);
}

double LinearRegressionStore::columnProductSum(
    int series, CalculationOptions options) const {
  double result = 0;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    double value0 = options.transformValue(get(series, 0, k), 0);
    double value1 = options.transformValue(get(series, 1, k), 1);
    result += value0 * value1;
  }
  return result;
}

double LinearRegressionStore::meanOfColumn(int series, int i,
                                           CalculationOptions options) const {
  return createDatasetFromColumn(series, i, options).mean();
}

double LinearRegressionStore::varianceOfColumn(
    int series, int i, CalculationOptions options) const {
  return createDatasetFromColumn(series, i, options).variance();
}

double LinearRegressionStore::standardDeviationOfColumn(
    int series, int i, CalculationOptions options) const {
  return createDatasetFromColumn(series, i, options).standardDeviation();
}

double LinearRegressionStore::sampleStandardDeviationOfColumn(
    int series, int i, CalculationOptions options) const {
  return createDatasetFromColumn(series, i, options).sampleStandardDeviation();
}

double LinearRegressionStore::covariance(int series,
                                         CalculationOptions options) const {
  double mean0 = meanOfColumn(series, 0, options);
  double mean1 = meanOfColumn(series, 1, options);
  return columnProductSum(series, options) / numberOfPairsOfSeries(series) -
         mean0 * mean1;
}

double LinearRegressionStore::slope(int series,
                                    CalculationOptions options) const {
  return LinearModelHelper::Slope(covariance(series, options),
                                  varianceOfColumn(series, 0, options));
}

double LinearRegressionStore::yIntercept(int series,
                                         CalculationOptions options) const {
  return LinearModelHelper::YIntercept(meanOfColumn(series, 1, options),
                                       meanOfColumn(series, 0, options),
                                       slope(series, options));
}

Poincare::StatisticsDataset<double>
LinearRegressionStore::createDatasetFromColumn(
    int series, int i, CalculationOptions options) const {
  Poincare::StatisticsDataset<double> dataset =
      Poincare::StatisticsDataset<double>(&m_dataLists[series][i],
                                          options.lnOfValue(i),
                                          options.oppositeOfValue(i));
  return dataset;
}

}  // namespace Shared
