#ifndef POINCARE_REGRESSION_DATASET_ADAPTER_H
#define POINCARE_REGRESSION_DATASET_ADAPTER_H

#include "data_table.h"
#include "statistics_dataset.h"

namespace Poincare::Internal {

class DatasetColumnAdapter : public Internal::DatasetColumn<double> {
 public:
  DatasetColumnAdapter(const DataTable* data, int column)
      : m_dataTable(data), m_column(column) {
    assert(m_dataTable->numberOfColumns() > m_column);
  }

  double valueAtIndex(int index) const override {
    return m_dataTable->get(m_column, index);
  }
  int length() const override { return m_dataTable->numberOfRows(); }

 private:
  const DataTable* m_dataTable;
  int m_column;
};

class StatisticsDatasetFromColumn : public StatisticsDataset<double> {
 public:
  StatisticsDatasetFromColumn(const DataTable* data, int column,
                              bool lnOfValues = false,
                              bool oppositeOfValues = false)
      : StatisticsDataset(&m_columnAdapter, lnOfValues, oppositeOfValues),
        m_columnAdapter(data, column) {}

 private:
  const DatasetColumnAdapter m_columnAdapter;
};

class StatisticsDatasetFromSeries : public StatisticsDataset<double> {
 public:
  StatisticsDatasetFromSeries(const DataTable* data, bool lnOfValues = false,
                              bool oppositeOfValues = false)
      : StatisticsDataset(&m_xAdapter, &m_yAdapter, lnOfValues,
                          oppositeOfValues),
        m_xAdapter(data, 0),
        m_yAdapter(data, 1) {
    assert(data->numberOfColumns() == 2);
  }

 private:
  const DatasetColumnAdapter m_xAdapter;
  const DatasetColumnAdapter m_yAdapter;
};

}  // namespace Poincare::Internal

#endif
