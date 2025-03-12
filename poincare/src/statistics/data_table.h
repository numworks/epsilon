#ifndef POINCARE_REGRESSION_SERIES_H
#define POINCARE_REGRESSION_SERIES_H

#include <assert.h>

#include "statistics_dataset.h"

namespace Poincare {
namespace Internal {

class StatisticsDatasetFromColumn;

class DataTable {
 public:
#if TARGET_POINCARE_JS
  virtual ~DataTable() = default;
#endif

  virtual int numberOfColumns() const = 0;
  virtual int numberOfRows() const = 0;

  virtual double get(int column, int row) const = 0;
  virtual void set(double value, int column, int row) { assert(false); }

  double totalSum() const;
  double rowSum(int row) const;
  double columnSum(int column) const;

  // Statistics
  StatisticsDatasetFromColumn createDatasetFromColumn(
      int i, StatisticsCalculationOptions options = {}) const;
  double meanOfColumn(int i, StatisticsCalculationOptions options) const;
  double varianceOfColumn(int i, StatisticsCalculationOptions options) const;
};

/* Series are a dataTable with 2 columns
 * They are mainly used in Regressions */
class Series : public DataTable {
 public:
  virtual int numberOfPairs() const = 0;

  int numberOfColumns() const override { return 2; }
  int numberOfRows() const override { return numberOfPairs(); }

  virtual double getX(int i) const = 0;
  virtual double getY(int i) const = 0;

  double get(int column, int row) const override {
    assert(column == 0 || column == 1);
    return column == 0 ? getX(row) : getY(row);
  }
  bool numberOfDistinctAbscissaeGreaterOrEqualTo(int i) const;

  // Statistics
  double columnProductSum(StatisticsCalculationOptions options = {}) const;
  double covariance(StatisticsCalculationOptions options = {}) const;
  double slope(StatisticsCalculationOptions options = {}) const;
  double yIntercept(StatisticsCalculationOptions options = {}) const;
};

}  // namespace Internal

}  // namespace Poincare
#endif
