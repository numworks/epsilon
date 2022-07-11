#ifndef SHARED_LINEAR_REGRESSION_STORE_H
#define SHARED_LINEAR_REGRESSION_STORE_H

#include "double_pair_store.h"
#include <poincare/statistics_dataset.h>
#include <float.h>

namespace Shared {

namespace LinearModelHelper {

double Slope(double covariance, double variance);
double YIntercept(double meanOfY, double meanOfX, double slope);

}

class LinearRegressionStore : public Shared::DoublePairStore {
public:
  constexpr static const char * const * k_columnNames = DoublePairStore::k_regressionColumNames;

  LinearRegressionStore(Shared::GlobalContext * context);

  // DoublePairStore
  char columnNamePrefixAtIndex(int column) const override {
    assert(column >= 0 && column < DoublePairStore::k_numberOfColumnsPerSeries);
    assert(strlen(k_columnNames[column]) == 1);
    return k_columnNames[column][0];
  }
  double defaultValue(int series, int i, int j) const override;

  // Calculation
  double doubleCastedNumberOfPairsOfSeries(int series) const;
  double squaredOffsettedValueSumOfColumn(int series, int i, bool lnOfSeries = false, double offset = 0) const;
  double squaredValueSumOfColumn(int series, int i, bool lnOfSeries = false) const;
  double leastSquaredSum(int series) const;
  double columnProductSum(int series, bool lnOfSeries = false) const;
  double meanOfColumn(int series, int i, bool lnOfSeries = false) const;
  double varianceOfColumn(int series, int i, bool lnOfSeries = false) const;
  double standardDeviationOfColumn(int series, int i, bool lnOfSeries = false) const;
  double sampleStandardDeviationOfColumn(int series, int i, bool lnOfSeries = false) const;
  double covariance(int series, bool lnOfSeries = false) const;
  double slope(int series, bool lnOfSeries = false) const;
  double yIntercept(int series, bool lnOfSeries = false) const;
  double correlationCoefficient(int series) const; // R

private:
  Poincare::StatisticsDataset<double> createDatasetFromColumn(int series, int i, bool lnOfSeries = false) const;
};

}

#endif
