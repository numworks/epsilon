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
                          weightsColumnIndex >= 0 ? &m_weightsAdapter : nullptr,
                          lnOfValues, oppositeOfValues),
        m_valuesAdapter(data, valuesColumnIndex),
        m_weightsAdapter(data, weightsColumnIndex) {}

  /* Copy operations needs to be defined so that m_values and m_weights point to
   * its own column adapters. */

  // Copy constructor
  StatisticsDatasetFromTable(const StatisticsDatasetFromTable& other)
      : StatisticsDataset(other),
        m_valuesAdapter(other.m_valuesAdapter),
        m_weightsAdapter(other.m_weightsAdapter) {
    // Adjust m_values and m_weights to point to the corresponding adapters
    m_values = &m_valuesAdapter;
    m_weights = other.m_weights ? &m_weightsAdapter : nullptr;
  }

  // Copy assignment operator
  StatisticsDatasetFromTable& operator=(
      const StatisticsDatasetFromTable& other) {
    if (this == &other) {
      return *this;
    }
    StatisticsDataset::operator=(other);
    m_valuesAdapter = other.m_valuesAdapter;
    m_weightsAdapter = other.m_weightsAdapter;
    // Adjust m_values and m_weights to point to the corresponding adapters
    m_values = &m_valuesAdapter;
    m_weights = other.m_weights ? &m_weightsAdapter : nullptr;
    return *this;
  }

  // Destructor not needed (rule of 3)
  ~StatisticsDatasetFromTable() = default;

 private:
  DatasetColumnAdapter m_valuesAdapter;
  DatasetColumnAdapter m_weightsAdapter;
};

}  // namespace Poincare::Internal

#endif
