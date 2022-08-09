#ifndef SHARED_DOUBLE_PAIR_STORE_H
#define SHARED_DOUBLE_PAIR_STORE_H

#include "global_context.h"
#include <escher/palette.h>
#include <kandinsky/color.h>
#include <poincare/float_list.h>
#include <poincare/range.h>
#include <algorithm>
#include <assert.h>
#include <stdint.h>

namespace Shared {

class DoublePairStorePreferences;

class DoublePairStore {
public:
  // 1 char for prefix, 1 char for index
  constexpr static int k_columnNamesLength = 2;
  constexpr static int k_numberOfSeries = 3;
  constexpr static int k_numberOfColumnsPerSeries = 2;
  constexpr static uint8_t k_maxNumberOfPairs = 100;
  // Must be 1 char long or change the name-related methods.
  constexpr static const char * k_regressionColumNames[] = {"X", "Y"};
  static_assert(sizeof(k_regressionColumNames) / sizeof(char *) == k_numberOfColumnsPerSeries, "Number of columns per series does not match number of column names in Regression.");
  // Must be 1 char long or change the name-related methods.
  constexpr static const char * k_statisticsColumNames[] = {"V", "N"};
  static_assert(sizeof(k_statisticsColumNames) / sizeof(char *) == k_numberOfColumnsPerSeries, "Number of columns per series does not match number of column names in Statistics.");

  DoublePairStore(GlobalContext * context, DoublePairStorePreferences * preferences);
  // Delete the implicit copy constructor: the object is heavy
  DoublePairStore(const DoublePairStore&) = delete;

  // Call this after initializing the store
  void initListsFromStorage(bool seriesShouldUpdate = true);

  // Clean the pool
  void tidy();

  // Column name
  virtual char columnNamePrefixAtIndex(int column) const = 0;
  // Fills 3 chars in the buffer (2 chars for name + null terminate)
  int fillColumnName(int series, int column, char * buffer) const;
  bool isColumnName(const char * name, int nameLen, int * returnSeries = nullptr, int * returnColumn = nullptr);

  // Get and set data
  double get(int series, int i, int j) const;
  bool set(double f, int series, int i, int j, bool delayUpdate = false, bool setOtherColumnToDefaultIfEmpty = false);
  bool setList(Poincare::List List, int series, int i, bool delayUpdate = false, bool setOtherColumnToDefaultIfEmpty = false);

  // Counts
  int numberOfPairs() const;
  uint8_t numberOfPairsOfSeries(int series) const {
    assert(series >= 0 && series < k_numberOfSeries);
    return std::max(lengthOfColumn(series, 0), lengthOfColumn(series, 1));
  }
  int lengthOfColumn(int series, int i) const {
    return m_dataLists[series][i].length();
  }

  // Delete and reset
  virtual bool deleteValueAtIndex(int series, int i, int j, bool authorizeNonEmptyRowDeletion, bool delayUpdate = false);
  void deletePairOfSeriesAtIndex(int series, int j, bool delayUpdate = false);
  void deleteColumn(int series, int i, bool delayUpdate = false);
  void resetColumn(int series, int i, bool delayUpdate = false);
  void deleteAllPairsOfSeries(int series, bool delayUpdate = false);
  void deleteAllPairs(bool delayUpdate = false);

  // Series validity
  void setSeriesHidden(int series, bool hidden);
  bool seriesIsActive(int series) const;
  bool seriesIsValid(int series) const;
  typedef bool (*ActiveSeriesTest)(const DoublePairStore *, int);
  static bool DefaultActiveSeriesTest(const DoublePairStore * store, int series) { return store->seriesIsActive(series); }
  bool hasActiveSeries(ActiveSeriesTest activeSeriesTest = &DefaultActiveSeriesTest) const;
  virtual void updateSeriesValidity(int series, bool updateDisplayAdditionalColumn = true);
  int numberOfActiveSeries(ActiveSeriesTest activeSeriesTest = &DefaultActiveSeriesTest) const;
  int indexOfKthActiveSeries(int k, ActiveSeriesTest activeSeriesTest = &DefaultActiveSeriesTest) const;

  // Series and columns
  virtual int relativeColumnIndex(int columnIndex) const { return columnIndex % k_numberOfColumnsPerSeries; }
  virtual int seriesAtColumn(int column) const { return column / k_numberOfColumnsPerSeries; }

  // Calculations
  void sortColumn(int series, int column, bool delayUpdate = false);
  void sortIndexByColumn(uint8_t * sortedIndex, int series, int column, int startIndex, int endIndex) const;
  double sumOfColumn(int series, int i, bool lnOfSeries = false) const;

  /* WARNING: This checksum is too slow. Avoid using it if you can.
  * Use it if you want to check that the list was modified outside this object
  * (through the modification of lists in calculation for example).
  * In other cases, you can use the method updateSeries, which is called
  * each time a series is modified during the lifecycle of the store object. */
  uint32_t storeChecksum() const;
  uint32_t storeChecksumForSeries(int series) const;

  // Colors
  static KDColor colorOfSeriesAtIndex(size_t i) {
    assert(i < k_numberOfSeries);
    assert(i < Escher::Palette::numberOfDataColors());
    return Escher::Palette::DataColor[i];
  }
  static KDColor colorLightOfSeriesAtIndex(size_t i) {
    assert(i < k_numberOfSeries);
    assert(i < Escher::Palette::numberOfLightDataColors());
    return Escher::Palette::DataColorLight[i];
  }
  /* This must be called each time the lists are modified.
   * It deletes the pairs of empty values and the trailing undef values,
   * updates the valid series, and stores the lists in the storage
   * */
  virtual bool updateSeries(int series, bool delayUpdate = false, bool updateDisplayAdditionalColumn = true); // TODO: find a better way than adding bool updateDisplayAdditionalColumn
  virtual bool valueValidInColumn(double value, int relativeColumn) const { return !std::isnan(value) && value >= -Poincare::Range1D::k_maxFloat && value <= Poincare::Range1D::k_maxFloat; }

protected:
  void initListsInPool();
  double defaultValue(int series, int i, int j) const;
  virtual double defaultValueForColumn1() const = 0;

  Poincare::FloatList<double> m_dataLists[k_numberOfSeries][k_numberOfColumnsPerSeries];
  DoublePairStorePreferences * m_storePreferences;

private:
  static_assert(k_maxNumberOfPairs <= UINT8_MAX, "k_maxNumberOfPairs is too large.");
  bool storeColumn(int series, int i) const;
  void deleteTrailingUndef(int series, int i);
  void deletePairsOfUndef(int series);

  GlobalContext * m_context;
};

}

#endif
