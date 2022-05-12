#ifndef SHARED_DOUBLE_PAIR_STORE_H
#define SHARED_DOUBLE_PAIR_STORE_H

#include <kandinsky/color.h>
#include <escher/palette.h>
#include <stdint.h>
#include <assert.h>
#include <poincare/list.h>
#include <poincare/complex.h>

namespace Shared {

class DoublePairStore {
public:
  constexpr static int k_columnNamesLength = 2; // 1 char for prefix, 1 char for index
  constexpr static int k_numberOfSeries = 3;
  constexpr static int k_numberOfColumnsPerSeries = 2;
  constexpr static uint8_t k_maxNumberOfPairs = 100;
  DoublePairStore();
  // Delete the implicit copy constructor: the object is heavy
  DoublePairStore(const DoublePairStore&) = delete;

  // Call this after initializing the store
  void initListsFromStorage();

  // Column name
  virtual char columnNamePrefixAtIndex(int column) const =0;
  int fillColumnName(int series, int column, char * buffer) const; // Fills 3 chars in the buffer (2 chars for name + null terminate)
  bool isColumnName(const char * name, int nameLen, int * returnSeries = nullptr, int * returnColumn = nullptr);

  // Get and set data
  double get(int series, int i, int j) const;
  virtual void set(double f, int series, int i, int j, bool setOtherColumnToDefaultIfEmpty = false);
  void setList(Poincare::List List, int series, int i);

  // Counts
  int numberOfPairs() const;
  uint8_t numberOfPairsOfSeries(int series) const {
    assert(series >= 0 && series < k_numberOfSeries);
    return std::max(lengthOfColumn(series, 0), lengthOfColumn(series, 1));
  }
  int lengthOfColumn(int series, int i) const {
    return m_dataLists[series][i].numberOfChildren();
  }

  // Delete and reset
  virtual bool deleteValueAtIndex(int series, int i, int j);
  virtual void deletePairOfSeriesAtIndex(int series, int j);
  void deleteColumn(int series, int i);
  virtual void resetColumn(int series, int i);
  virtual void deleteAllPairsOfSeries(int series);
  void deleteAllPairs();

  // Series validity
  void hideSeries(int series) { m_validSeries[series] = false; }
  bool seriesIsValid(int series) const;
  typedef bool (*ValidSeries)(const DoublePairStore *, int);
  static bool DefaultValidSeries(const DoublePairStore * store, int series) { return store->seriesIsValid(series); }
  // These methods can be implemented with a different validity method
  bool hasValidSeries(ValidSeries = &DefaultValidSeries) const;
  virtual void updateSeriesValidity(int series);
  int numberOfValidSeries(ValidSeries = &DefaultValidSeries) const;
  int indexOfKthValidSeries(int k, ValidSeries = &DefaultValidSeries) const;

  // Series and columns
  virtual int relativeColumnIndex(int columnIndex) const { return columnIndex % k_numberOfColumnsPerSeries; }
  virtual int seriesAtColumn(int column) const { return column / k_numberOfColumnsPerSeries; }

  // Calculations
  virtual void sortColumn(int series, int column);
  void sortIndexByColumn(uint8_t * sortedIndex, int series, int column, int startIndex, int endIndex) const;
  double sumOfColumn(int series, int i, bool lnOfSeries = false) const;
  bool seriesNumberOfAbscissaeGreaterOrEqualTo(int series, int i) const;
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

protected:
  virtual double defaultValue(int series, int i, int j) const;
  /* This must be called each time the lists are modified.
   * It deletes the pairs of empty values and the trailing undef values,
   * updates the valid series, and stores the lists in the storage
   * */
  void updateSeries(int series);
  /* Since sometimes we do multiple list modification in a row, we don't want
   * to update the series each time, but only at the end of the modifications.
   * So these methods are called to set a flag preventing the update.*/
  void preventUpdate() {
    m_updateFlag++;
  }
  void enableUpdate() {
    assert(m_updateFlag > 0);
    m_updateFlag--;
  }
  bool m_validSeries[k_numberOfSeries];
private:
  static_assert(k_maxNumberOfPairs <= UINT8_MAX, "k_maxNumberOfPairs is too large.");
  void storeColumn(int series, int i) const;
  void deleteTrailingUndef(int series, int i);
  void deletePairsOfUndef(int series);

  Poincare::List m_dataLists[k_numberOfSeries][k_numberOfColumnsPerSeries];
  int m_updateFlag;
};

}

#endif
