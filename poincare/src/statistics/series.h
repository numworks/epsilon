#ifndef POINCARE_REGRESSION_SERIES_H
#define POINCARE_REGRESSION_SERIES_H

#include <assert.h>

#include "statistics_dataset.h"

namespace Poincare {
namespace Internal {

class StatisticsDatasetFromSeriesColumn;

// Apps will provide series to the regression algorithm with Series
class Series {
 public:
  virtual double getX(int i) const = 0;
  virtual double getY(int i) const = 0;

#if TARGET_POINCARE_JS
  virtual ~Series() = default;
#endif

  double get(int column, int i) const {
    assert(column == 0 || column == 1);
    return column == 0 ? getX(i) : getY(i);
  }
  virtual int numberOfPairs() const = 0;
  bool numberOfDistinctAbscissaeGreaterOrEqualTo(int i) const;

  // Statistics
  StatisticsDatasetFromSeriesColumn createDatasetFromColumn(
      int i, StatisticsCalculationOptions options = {}) const;
  double columnProductSum(StatisticsCalculationOptions options = {}) const;
  double covariance(StatisticsCalculationOptions options = {}) const;
  double slope(StatisticsCalculationOptions options = {}) const;
  double yIntercept(StatisticsCalculationOptions options = {}) const;

 private:
  double meanOfColumn(int i, StatisticsCalculationOptions options) const;
  double varianceOfColumn(int i, StatisticsCalculationOptions options) const;
};

}  // namespace Internal

}  // namespace Poincare
#endif
