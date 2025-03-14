#ifndef POINCARE_STATISTICS_STATISTICS_DATASET_H
#define POINCARE_STATISTICS_STATISTICS_DATASET_H

#include <poincare/src/statistics/data_table.h>
#include <poincare/src/statistics/dataset_adapter.h>
#include <poincare/src/statistics/statistics_dataset.h>
#include <poincare/src/statistics/statistics_dataset_column.h>

namespace Poincare {

using Internal::ConstantDatasetColumn;
using Internal::DatasetColumn;

using Internal::StatisticsCalculationOptions;

using Internal::DataTable;
using Internal::Series;

// Cf internal documentation
using Internal::StatisticsDataset;
using Internal::StatisticsDatasetFromTable;

}  // namespace Poincare
#endif
