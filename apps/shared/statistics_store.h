#ifndef SHARED_STATISTICS_STORE_H
#define SHARED_STATISTICS_STORE_H

#include <poincare/statistics_dataset.h>

#include "double_pair_store.h"

namespace Shared {

class StatisticsStore : public DoublePairStore {
 public:
  constexpr static const char* const* k_columnNames =
      DoublePairStore::k_statisticsColumNames;
  constexpr static char k_tableName[] = "V%c/N%c";
  constexpr static size_t k_tableNameSize = sizeof(k_tableName);

  StatisticsStore(GlobalContext* context,
                  DoublePairStorePreferences* preferences);

  void invalidateSortedIndexes();
  void clearColumn(int series, int column);
  double sumOfOccurrences(int series) const;
  double mean(int series) const;
  double standardDeviation(int series) const;
  double sampleStandardDeviation(int series) const;

  // DoublePairStore
  char columnNamePrefixAtIndex(int column) const override {
    assert(column >= 0 && column < k_numberOfColumnsPerSeries);
    assert(strlen(k_columnNames[column]) == 1);
    return k_columnNames[column][0];
  }
  double defaultValueForColumn1() const override { return 1.0; }
  bool updateSeries(int series, bool delayUpdate = false) override;
  bool valueValidInColumn(double value, int relativeColumn) const override {
    return DoublePairStore::valueValidInColumn(value, relativeColumn) &&
           (relativeColumn != 1 || value >= 0.0);
  }
  void fillTableName(int series, char* buffer, size_t bufferSize) const;

 protected:
  void initDatasets();
  void tidyDatasets();

  // Sorted value indexes are memoized to save computation
  static_assert(k_maxNumberOfPairs <= UINT8_MAX,
                "k_maxNumberOfPairs is too large.");
  /* The dataset memoizes the sorted indexes */
  Poincare::StatisticsDataset<double> m_datasets[k_numberOfSeries];
};

}  // namespace Shared

#endif
