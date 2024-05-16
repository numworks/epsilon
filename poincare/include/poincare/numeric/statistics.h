#ifndef POINCARE_NUMERIC_STATISTICS_H
#define POINCARE_NUMERIC_STATISTICS_H

#include <poincare/src/numeric/statistics_dataset.h>
#include <poincare/src/numeric/statistics_dataset_column.h>

namespace Poincare {

template <typename T>
using DatasetColumn = Internal::DatasetColumn<T>;

template <typename T>
using ConstantDatasetColumn = Internal::ConstantDatasetColumn<T>;

// Cf internal documentation
template <typename T>
using StatisticsDataset = Internal::StatisticsDataset<T>;

}  // namespace Poincare
#endif
