#include "linear_regression_store.h"
#include <poincare/preferences.h>
#include <assert.h>
#include <float.h>
#include <cmath>
#include <string.h>
#include <algorithm>

namespace Shared {

/* Linear model helper */
double LinearModelHelper::Slope(double covariance, double variance) {
  return covariance / variance;
}

double LinearModelHelper::YIntercept(double meanOfY, double meanOfX, double slope) {
  return meanOfY - slope * meanOfX;
}

/* Linear Regression Store */

LinearRegressionStore::LinearRegressionStore(GlobalContext * context) :
  DoublePairStore(context)
{}

/* Calculations */

double LinearRegressionStore::doubleCastedNumberOfPairsOfSeries(int series) const {
  return DoublePairStore::numberOfPairsOfSeries(series);
}

double LinearRegressionStore::squaredOffsettedValueSumOfColumn(int series, int i, bool lnOfSeries, double offset) const {
  return createDatasetFromColumn(series, i, lnOfSeries).offsettedSquaredSum(offset);
}

double LinearRegressionStore::squaredValueSumOfColumn(int series, int i, bool lnOfSeries) const {
  return squaredOffsettedValueSumOfColumn(series, i, lnOfSeries, 0.0);
}

double LinearRegressionStore::leastSquaredSum(int series) const {
  Poincare::StatisticsDataset<double> xDataset = createDatasetFromColumn(series, 0);
  Poincare::StatisticsDataset<double> yDataset = createDatasetFromColumn(series, 1);
  // Find the linear regression
  double a = yIntercept(series);
  double b = slope(series);
  return yDataset.squaredSumOffsettedByLinearTransformationOfDataset(xDataset, a, b);
}

double LinearRegressionStore::columnProductSum(int series, bool lnOfSeries) const {
  double result = 0;
  int numberOfPairs = numberOfPairsOfSeries(series);
  for (int k = 0; k < numberOfPairs; k++) {
    double value0 = get(series,0,k);
    double value1 = get(series,1,k);
    if (lnOfSeries) {
      value0 = log(value0);
      value1 = log(value1);
    }
    result += value0 * value1;
  }
  return result;
}

double LinearRegressionStore::meanOfColumn(int series, int i, bool lnOfSeries) const {
  return createDatasetFromColumn(series, i, lnOfSeries).mean();
}

double LinearRegressionStore::varianceOfColumn(int series, int i, bool lnOfSeries) const {
  return createDatasetFromColumn(series, i, lnOfSeries).variance();
}

double LinearRegressionStore::standardDeviationOfColumn(int series, int i, bool lnOfSeries) const {
  return createDatasetFromColumn(series, i, lnOfSeries).standardDeviation();
}

double LinearRegressionStore::sampleStandardDeviationOfColumn(int series, int i, bool lnOfSeries) const {
  return createDatasetFromColumn(series, i, lnOfSeries).sampleStandardDeviation();
}

double LinearRegressionStore::covariance(int series, bool lnOfSeries) const {
  double mean0 = meanOfColumn(series, 0, lnOfSeries);
  double mean1 = meanOfColumn(series, 1, lnOfSeries);
  return columnProductSum(series, lnOfSeries)/numberOfPairsOfSeries(series) - mean0 * mean1;
}

double LinearRegressionStore::slope(int series, bool lnOfSeries) const {
  return LinearModelHelper::Slope(covariance(series, lnOfSeries), varianceOfColumn(series, 0, lnOfSeries));
}

double LinearRegressionStore::yIntercept(int series, bool lnOfSeries) const {
  return LinearModelHelper::YIntercept(meanOfColumn(series, 1, lnOfSeries), meanOfColumn(series, 0, lnOfSeries), slope(series, lnOfSeries));
}

double LinearRegressionStore::correlationCoefficient(int series) const {
  /* Returns the correlation coefficient (R) between the series X and Y.
   * In non-linear regressions, its square is different from the determinationCoefficient
   * It is usually displayed in linear regressions only to avoid confusion */
  double v0 = varianceOfColumn(series, 0);
  double v1 = varianceOfColumn(series, 1);
  return (v0 == 0.0 || v1 == 0.0) ? 1.0 : covariance(series) / std::sqrt(v0 * v1);
}

Poincare::StatisticsDataset<double> LinearRegressionStore::createDatasetFromColumn(int series, int i, bool lnOfSeries) const {
  Poincare::StatisticsDataset<double> dataset = Poincare::StatisticsDataset<double>(&m_dataLists[series][i]);
  dataset.setLnOfValues(lnOfSeries);
  return dataset;
}

}

