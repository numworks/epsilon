#ifndef SHARED_LINEAR_REGRESSION_STORE_H
#define SHARED_LINEAR_REGRESSION_STORE_H

#include <float.h>
#include <poincare/statistics_dataset.h>

#include "double_pair_store.h"

namespace Shared {

namespace LinearModelHelper {

double Slope(double covariance, double variance);
double YIntercept(double meanOfY, double meanOfX, double slope);

}  // namespace LinearModelHelper

class LinearRegressionStore : public Shared::DoublePairStore {
 public:
  constexpr static const char* const* k_columnNames =
      DoublePairStore::k_regressionColumNames;

  LinearRegressionStore(Shared::GlobalContext* context,
                        DoublePairStorePreferences* preferences);

  // DoublePairStore
  char columnNamePrefixAtIndex(int column) const override {
    assert(column >= 0 && column < DoublePairStore::k_numberOfColumnsPerSeries);
    assert(strlen(k_columnNames[column]) == 1);
    return k_columnNames[column][0];
  }
  double defaultValueForColumn1() const override { return 0.0; }

  // Calculation
  double doubleCastedNumberOfPairsOfSeries(int series) const;
  double squaredOffsettedValueSumOfColumn(
      int series, int i, double offset = 0,
      CalculationOptions options = CalculationOptions()) const;
  double squaredValueSumOfColumn(
      int series, int i,
      CalculationOptions options = CalculationOptions()) const;
  double leastSquaredSum(int series) const;
  double columnProductSum(int series, CalculationOptions options) const;
  double columnProductSum(int series) const {
    return columnProductSum(series, CalculationOptions());
  }
  double meanOfColumn(int series, int i,
                      CalculationOptions options = CalculationOptions()) const;
  double varianceOfColumn(
      int series, int i,
      CalculationOptions options = CalculationOptions()) const;
  double standardDeviationOfColumn(
      int series, int i,
      CalculationOptions options = CalculationOptions()) const;
  double sampleStandardDeviationOfColumn(
      int series, int i,
      CalculationOptions options = CalculationOptions()) const;
  double covariance(int series, CalculationOptions options) const;
  double covariance(int series) const {
    return covariance(series, CalculationOptions());
  }
  double slope(int series,
               CalculationOptions options = CalculationOptions()) const;
  double yIntercept(int series,
                    CalculationOptions options = CalculationOptions()) const;

 private:
  Poincare::StatisticsDataset<double> createDatasetFromColumn(
      int series, int i,
      CalculationOptions options = CalculationOptions()) const;
};

}  // namespace Shared

#endif
