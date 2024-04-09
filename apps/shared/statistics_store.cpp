#include "statistics_store.h"

#include <poincare/print.h>

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
  if (column == 0) {
    deleteAllPairsOfSeries(series);
  } else {
    assert(column == 1);
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

size_t StatisticsStore::clearPopUpText(int column, char* buffer,
                                       size_t bufferSize) const {
  int series = seriesAtColumn(column);
  int relativeCol = relativeColumn(column);
  assert(relativeCol == 0 || relativeCol == 1);
  constexpr size_t placeHolderSize = DoublePairStore::k_tableNameLength + 1;
  char placeHolder[placeHolderSize];
  I18n::Message message = I18n::Message::Default;
  if (relativeCol == 0) {
    tableName(series, placeHolder, placeHolderSize);
    message = I18n::Message::ClearTableConfirmation;
  } else {
    fillColumnName(series, relativeCol, placeHolder);
    message = I18n::Message::ResetFreqConfirmation;
  }
  return Poincare::Print::CustomPrintf(buffer, bufferSize,
                                       I18n::translate(message), placeHolder);
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
