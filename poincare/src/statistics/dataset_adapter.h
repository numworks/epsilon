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
    assert(m_column >= 0);
    return m_dataTable->get(m_column, index);
  }
  int length() const override {
    assert(m_column >= 0);
    return m_dataTable->numberOfRows();
  }

 private:
  const DataTable* m_dataTable;
  int m_column;
};

class StatisticsDatasetFromTable : public StatisticsDataset<double> {
 public:
  // Use weightsColumnIndex = -1 to create a dataset with all weights equal to 1
  StatisticsDatasetFromTable(const DataTable* data, int valuesColumnIndex,
                             int weightsColumnIndex = -1,
                             bool lnOfValues = false,
                             bool oppositeOfValues = false)
      : StatisticsDataset(&m_valuesAdapter,
                          weightsColumnIndex >= 0 ? &m_weigthsAdapter : nullptr,
                          lnOfValues, oppositeOfValues),
        m_valuesAdapter(data, valuesColumnIndex),
        m_weigthsAdapter(data, weightsColumnIndex) {}

 private:
  const DatasetColumnAdapter m_valuesAdapter;
  const DatasetColumnAdapter m_weigthsAdapter;
};

}  // namespace Poincare::Internal

#endif
