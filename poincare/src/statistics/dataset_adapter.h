#ifndef POINCARE_REGRESSION_DATASET_ADAPTER_H
#define POINCARE_REGRESSION_DATASET_ADAPTER_H

#include "series.h"
#include "statistics_dataset.h"

namespace Poincare::Internal {

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

class StatisticsDatasetFromSeriesColumn : public StatisticsDataset<double> {
 public:
  StatisticsDatasetFromSeriesColumn(const Series* series, int column,
                                    bool lnOfValues = false,
                                    bool oppositeOfValues = false)
      : StatisticsDataset(&m_columnAdapter, lnOfValues, oppositeOfValues),
        m_columnAdapter(series, column) {}

 private:
  const DatasetColumnSeriesAdapter m_columnAdapter;
};

}  // namespace Poincare::Internal

#endif
