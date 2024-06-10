#ifndef POINCARE_REGRESSION_DATASET_ADAPTER_H
#define POINCARE_REGRESSION_DATASET_ADAPTER_H

#include <poincare/src/numeric/statistics_dataset.h>
#include <poincare/src/numeric/statistics_dataset_column.h>

#include "regression.h"

namespace Poincare::Regression {

class DatasetColumnSeriesAdapter : public Internal::DatasetColumn<double> {
 public:
  DatasetColumnSeriesAdapter(const Series* series, int column)
      : m_series(series), m_column(column) {
    assert(column == 0 || column == 1);
  }

  double valueAtIndex(int index) const override {
    return m_column == 0 ? m_series->getX(index) : m_series->getY(index);
  }
  int length() const override { return m_series->numberOfPairs(); }

 private:
  const Series* m_series;
  int m_column;
};

class DatasetSeriesAdapter : public StatisticsDataset<double> {
 public:
  DatasetSeriesAdapter(const Series* series)
      : StatisticsDataset(&m_xAdapter, &m_yAdapter),
        m_xAdapter(series, 0),
        m_yAdapter(series, 1) {}

 private:
  const DatasetColumnSeriesAdapter m_xAdapter;
  const DatasetColumnSeriesAdapter m_yAdapter;
};

}  // namespace Poincare::Regression

#endif
