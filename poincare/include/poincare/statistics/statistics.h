#ifndef POINCARE_STATISTICS_STATISTICS_DATASET_H
#define POINCARE_STATISTICS_STATISTICS_DATASET_H

#include <poincare/src/statistics/data_table.h>
#include <poincare/src/statistics/statistics_dataset.h>
#include <poincare/src/statistics/statistics_dataset_column.h>

namespace Poincare {

template <typename T>
using DatasetColumn = Internal::DatasetColumn<T>;

template <typename T>
using ConstantDatasetColumn = Internal::ConstantDatasetColumn<T>;

using StatisticsCalculationOptions = Internal::StatisticsCalculationOptions;

// Cf internal documentation
template <typename T>
using StatisticsDataset = Internal::StatisticsDataset<T>;

using DataTable = Internal::DataTable;
using Series = Internal::Series;

}  // namespace Poincare
#endif
