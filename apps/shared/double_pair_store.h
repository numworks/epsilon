#ifndef SHARED_DOUBLE_PAIR_STORE_H
#define SHARED_DOUBLE_PAIR_STORE_H

#include <assert.h>
#include <escher/palette.h>
#include <kandinsky/color.h>
#include <poincare/old/float_list.h>
#include <poincare/range.h>
#include <stdint.h>

#include <algorithm>
#include <array>

#include "global_context.h"

namespace Shared {

class DoublePairStorePreferences;

class DoublePairStore {
 public:
  // 1 char for prefix, 1 char for index
  constexpr static int k_columnNamesLength = 2;
  constexpr static int k_numberOfSeries = 6;
  constexpr static int k_numberOfColumnsPerSeries = 2;
  constexpr static uint8_t k_maxNumberOfPairs = 100;
  // Must be 1 char long or change the name-related methods.
  constexpr static const char* k_regressionColumNames[] = {"X", "Y"};
  static_assert(std::size(k_regressionColumNames) == k_numberOfColumnsPerSeries,
                "Number of columns per series does not match number of column "
                "names in Regression.");
  // Must be 1 char long or change the name-related methods.
  constexpr static const char* k_statisticsColumNames[] = {"V", "N"};
  static_assert(std::size(k_statisticsColumNames) == k_numberOfColumnsPerSeries,
                "Number of columns per series does not match number of column "
                "names in Statistics.");
  constexpr static size_t k_tableNameLength =
      2 * k_columnNamesLength + sizeof("/");

  DoublePairStore(GlobalContext* context,
                  DoublePairStorePreferences* preferences);
  // Delete the implicit copy constructor: the object is heavy
  DoublePairStore(const DoublePairStore&) = delete;

  // Call this after initializing the store
  void initListsFromStorage(bool delayUpdate = false);

  // Clean the pool
  void tidy();

  // Column name
  virtual char columnNamePrefixAtIndex(int column) const = 0;
  // Fills 3 chars in the buffer (2 chars for name + null terminate)
  size_t fillColumnName(int series, int column, char* buffer) const;
  bool isColumnName(const char* name, int nameLen, int* returnSeries = nullptr,
                    int* returnColumn = nullptr);
  size_t tableName(int series, char* buffer, size_t bufferSize) const;

  // Get and set data
  double get(int series, int i, int j) const;
  bool set(double f, int series, int i, int j, bool delayUpdate = false,
           bool setOtherColumnToDefaultIfEmpty = false);
  /* TODO: The "delayUpdate" parameter is kept for consistency but is currently
   * always called with true. Should it be removed ? */
  bool setList(Poincare::List& List, int series, int i,
               bool delayUpdate = false,
               bool setOtherColumnToDefaultIfEmpty = false);

  // Counts
  int numberOfPairs() const;
  uint8_t numberOfPairsOfSeries(int series) const {
    assert(series >= 0 && series < k_numberOfSeries);
    return std::max(lengthOfColumn(series, 0), lengthOfColumn(series, 1));
  }
  int lengthOfColumn(int series, int i) const {
    assert(series >= 0 && series < k_numberOfSeries && i >= 0 &&
           i < k_numberOfColumnsPerSeries);
    return m_dataLists[series][i].length();
  }

  // Delete and reset
  virtual bool deleteValueAtIndex(int series, int i, int j,
                                  bool authorizeNonEmptyRowDeletion,
                                  bool delayUpdate = false);
  void deletePairOfSeriesAtIndex(int series, int j, bool delayUpdate = false);
  void deleteColumn(int series, int i, bool delayUpdate = false);
  void resetColumn(int series, int i, bool delayUpdate = false);
  void deleteAllPairsOfSeries(int series, bool delayUpdate = false);
  void deleteAllPairs(bool delayUpdate = false);

  // Series validity
  void setSeriesHidden(int series, bool hidden);
  bool seriesIsActive(int series) const;
  bool seriesIsValid(int series) const;
  typedef bool (*ActiveSeriesTest)(const DoublePairStore*, int);
  static bool DefaultActiveSeriesTest(const DoublePairStore* store,
                                      int series) {
    return store->seriesIsActive(series);
  }
  bool hasActiveSeries(
      ActiveSeriesTest activeSeriesTest = &DefaultActiveSeriesTest) const;
  virtual void updateSeriesValidity(int series);
  int numberOfActiveSeries(
      ActiveSeriesTest activeSeriesTest = &DefaultActiveSeriesTest) const;
  int seriesIndexFromActiveSeriesIndex(
      int activeSeriesIndex,
      ActiveSeriesTest activeSeriesTest = &DefaultActiveSeriesTest) const;
  int activeSeriesIndexFromSeriesIndex(
      int seriesIndex,
      ActiveSeriesTest activeSeriesTest = &DefaultActiveSeriesTest) const;

  // Series and columns
  virtual int relativeColumn(int column) const {
    return column % k_numberOfColumnsPerSeries;
  }
  virtual int seriesAtColumn(int column) const {
    return column / k_numberOfColumnsPerSeries;
  }

  using CalculationOptions = Poincare::StatisticsCalculationOptions;

  void sortColumn(int series, int column, bool delayUpdate = false);
  double sumOfColumn(int series, int i,
                     CalculationOptions options = CalculationOptions()) const;

  /* WARNING: This checksum is too slow. Avoid using it if you can.
   * Use it if you want to check that the list was modified outside this object
   * (through the modification of lists in calculation for example).
   * In other cases, you can use the method updateSeries, which is called
   * each time a series is modified during the lifecycle of the store object. */
  uint32_t storeChecksum() const;
  uint32_t storeChecksumForSeries(int series) const;

  // Colors
  static KDColor colorOfSeriesAtIndex(size_t i) {
    static_assert(Escher::Palette::numberOfDataColors() >= k_numberOfSeries);
    assert(i < k_numberOfSeries);
    return Escher::Palette::DataColor[i];
  }
  static KDColor colorLightOfSeriesAtIndex(size_t i) {
    static_assert(Escher::Palette::numberOfLightDataColors() >=
                  k_numberOfSeries);
    assert(i < k_numberOfSeries);
    return Escher::Palette::DataColorLight[i];
  }
  /* This must be called each time the lists are modified.
   * It deletes the pairs of empty values and the trailing undef values,
   * updates the valid series, and stores the lists in the storage. */
  virtual bool updateSeries(int series, bool delayUpdate = false);
  virtual bool valueValidInColumn(double value, int relativeColumn) const {
    return !std::isnan(value) &&
           value >= -Poincare::Range1D<float>::k_maxFloat &&
           value <= Poincare::Range1D<float>::k_maxFloat;
  }

  bool listWasErasedByException(int i) const {
    return m_dataLists[i][0].wasErasedByException() ||
           m_dataLists[i][1].wasErasedByException();
  }

  enum class ClearType { ClearColumn, ClearTable, ResetColumn };
  virtual ClearType clearType(int relativeColumn) const {
    return ClearType::ClearColumn;
  }

 protected:
  void initListsInPool();
  double defaultValue(int series, int i, int j) const;
  virtual double defaultValueForColumn1() const = 0;

  Poincare::FloatList<double> m_dataLists[k_numberOfSeries]
                                         [k_numberOfColumnsPerSeries];
  DoublePairStorePreferences* m_storePreferences;

 private:
  static_assert(k_maxNumberOfPairs <= UINT8_MAX,
                "k_maxNumberOfPairs is too large.");
  bool storeColumn(int series, int i) const;
  void deleteTrailingUndef(int series, int i);
  void deletePairsOfUndef(int series);

  GlobalContext* m_context;
};

}  // namespace Shared

#endif
