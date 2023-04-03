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
    int series, int i, double offset, Parameters parameters) const {
  return createDatasetFromColumn(series, i, parameters)
      .offsettedSquaredSum(offset);
}

double LinearRegressionStore::squaredValueSumOfColumn(
    int series, int i, Parameters parameters) const {
  return squaredOffsettedValueSumOfColumn(series, i, 0.0, parameters);
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

double LinearRegressionStore::columnProductSum(int series,
                                               Parameters parameters) const {
  double result = 0;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    double value0 = parameters.transformValue(get(series, 0, k), 0);
    double value1 = parameters.transformValue(get(series, 1, k), 1);
    result += value0 * value1;
  }
  return result;
}

double LinearRegressionStore::meanOfColumn(int series, int i,
                                           Parameters parameters) const {
  return createDatasetFromColumn(series, i, parameters).mean();
}

double LinearRegressionStore::varianceOfColumn(int series, int i,
                                               Parameters parameters) const {
  return createDatasetFromColumn(series, i, parameters).variance();
}

double LinearRegressionStore::standardDeviationOfColumn(
    int series, int i, Parameters parameters) const {
  return createDatasetFromColumn(series, i, parameters).standardDeviation();
}

double LinearRegressionStore::sampleStandardDeviationOfColumn(
    int series, int i, Parameters parameters) const {
  return createDatasetFromColumn(series, i, parameters)
      .sampleStandardDeviation();
}

double LinearRegressionStore::covariance(int series,
                                         Parameters parameters) const {
  double mean0 = meanOfColumn(series, 0, parameters);
  double mean1 = meanOfColumn(series, 1, parameters);
  return columnProductSum(series, parameters) / numberOfPairsOfSeries(series) -
         mean0 * mean1;
}

double LinearRegressionStore::slope(int series, Parameters parameters) const {
  return LinearModelHelper::Slope(covariance(series, parameters),
                                  varianceOfColumn(series, 0, parameters));
}

double LinearRegressionStore::yIntercept(int series,
                                         Parameters parameters) const {
  return LinearModelHelper::YIntercept(meanOfColumn(series, 1, parameters),
                                       meanOfColumn(series, 0, parameters),
                                       slope(series, parameters));
}

Poincare::StatisticsDataset<double>
LinearRegressionStore::createDatasetFromColumn(int series, int i,
                                               Parameters parameters) const {
  Poincare::StatisticsDataset<double> dataset =
      Poincare::StatisticsDataset<double>(&m_dataLists[series][i],
                                          parameters.lnOfValue(i),
                                          parameters.oppositeOfValue(i));
  return dataset;
}

}  // namespace Shared
