#ifndef POINCARE_REGRESSION_DATASET_ADAPTER_H
#define POINCARE_REGRESSION_DATASET_ADAPTER_H

#include <poincare/numeric/statistics.h>

#include "series.h"

namespace Poincare::Regression {

class DatasetColumnSeriesAdapter : public Internal::DatasetColumn<double> {
 public:
  DatasetColumnSeriesAdapter(const Series* series, int column)
      : m_series(series), m_column(column) {
    assert(column == 0 || column == 1);
  }

  double valueAtIndex(int index) const override {
    return m_series->get(m_column, index);
  }
  int length() const override { return m_series->numberOfPairs(); }

 private:
  const Series* m_series;
  int m_column;
};

class OneColumn : public StatisticsDataset<double> {
 public:
  OneColumn(const Series* series, int column)
      : StatisticsDataset(&m_columnAdapter), m_columnAdapter(series, column) {}

 private:
  const DatasetColumnSeriesAdapter m_columnAdapter;
};

class DatasetSeriesAdapter : public StatisticsDataset<double> {
 public:
  DatasetSeriesAdapter(const Series* series)
      : StatisticsDataset(&m_xAdapter, &m_yAdapter),
        m_xAdapter(series, 0),
        m_yAdapter(series, 1) {}

  double columnProductSum(StatisticsCalculationOptions options) const {
    double result = 0;
    int numberOfPairs = numberOfPairsOfSeries();
    for (int k = 0; k < numberOfPairs; k++) {
      double value0 = options.transformValue(m_xAdapter.valueAtIndex(k), 0);
      double value1 = options.transformValue(m_yAdapter.valueAtIndex(k), 1);
      result += value0 * value1;
    }
    return result;
  }

  double covariance(StatisticsCalculationOptions options) const {
    double mean0 = meanOfColumn(0, options);
    double mean1 = meanOfColumn(1, options);
    return columnProductSum(options) / numberOfPairsOfSeries() - mean0 * mean1;
  }

  double numberOfPairsOfSeries() const { return m_xAdapter.length(); }

  double slope(StatisticsCalculationOptions options = {}) const {
    return covariance(options) / varianceOfColumn(0, options);
  }

  double yIntercept(StatisticsCalculationOptions options = {}) const {
    double meanOfX = meanOfColumn(0, options);
    double meanOfY = meanOfColumn(1, options);
    return meanOfY - slope(options) * meanOfX;
  }

  double meanOfColumn(int i, StatisticsCalculationOptions options) const {
    return createDatasetFromColumn(i, options).mean();
  }

  double varianceOfColumn(int i, StatisticsCalculationOptions options) const {
    return createDatasetFromColumn(i, options).variance();
  }

  Poincare::Internal::StatisticsDataset<double> createDatasetFromColumn(
      int i, StatisticsCalculationOptions options) const {
    return Poincare::Internal::StatisticsDataset<double>(
        i == 0 ? &m_xAdapter : &m_yAdapter, options.lnOfValue(i),
        options.oppositeOfValue(i));
  }

 private:
  const DatasetColumnSeriesAdapter m_xAdapter;
  const DatasetColumnSeriesAdapter m_yAdapter;
};

}  // namespace Poincare::Regression

#endif
