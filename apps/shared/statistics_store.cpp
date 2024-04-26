#include "statistics_store.h"

#include <poincare/old/print.h>

namespace Shared {

StatisticsStore::StatisticsStore(GlobalContext* context,
                                 DoublePairStorePreferences* preferences)
    : DoublePairStore(context, preferences) {}

void StatisticsStore::invalidateSortedIndexes() {
  for (int i = 0; i < k_numberOfSeries; i++) {
    m_datasets[i].setHasBeenModified();
  }
}

void StatisticsStore::clearColumn(int series, int column) {
  ClearType type = clearType(column);
  if (type == ClearType::ClearTable) {
    deleteAllPairsOfSeries(series);
  } else {
    assert(type == ClearType::ResetColumn);
    // Column won't be empty, all values are set to 1
    resetColumn(series, column);
  }
}

double StatisticsStore::sumOfOccurrences(int series) const {
  return m_datasets[series].totalWeight();
}

double StatisticsStore::mean(int series) const {
  return m_datasets[series].mean();
}

double StatisticsStore::standardDeviation(int series) const {
  return m_datasets[series].standardDeviation();
}

double StatisticsStore::sampleStandardDeviation(int series) const {
  return m_datasets[series].sampleStandardDeviation();
}

bool StatisticsStore::updateSeries(int series, bool delayUpdate) {
  m_datasets[series].setHasBeenModified();
  return DoublePairStore::updateSeries(series, delayUpdate);
}

void StatisticsStore::initDatasets() {
  /* Update series after having set the datasets, which are needed in
   * updateSeries */
  initListsFromStorage(true);
  for (int s = 0; s < k_numberOfSeries; s++) {
    m_datasets[s] = Poincare::StatisticsDataset<double>(&m_dataLists[s][0],
                                                        &m_dataLists[s][1]);
    updateSeries(s);
  }
}

void StatisticsStore::tidyDatasets() {
  for (int s = 0; s < k_numberOfSeries; s++) {
    m_datasets[s].tidyPool();
    for (int c = 0; c < k_numberOfColumnsPerSeries; c++) {
      m_dataLists[s][c] = Poincare::FloatList<double>();
    }
  }
}

}  // namespace Shared
