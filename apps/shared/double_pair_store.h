#ifndef SHARED_DOUBLE_PAIR_STORE_H
#define SHARED_DOUBLE_PAIR_STORE_H

#include <kandinsky/color.h>
#include <escher/palette.h>
#include <stdint.h>
#include <assert.h>

namespace Shared {

class DoublePairStore {
public:
  constexpr static int k_numberOfSeries = 3;
  constexpr static int k_numberOfColumnsPerSeries = 2;
  constexpr static int k_maxNumberOfPairs = 100;
  DoublePairStore() :
    m_data{},
    m_numberOfPairs{}
  {}
  // Delete the implicit copy constructor: the object is heavy
  DoublePairStore(const DoublePairStore&) = delete;

  // Get and set data
  double get(int series, int i, int j) const {
    assert(j < m_numberOfPairs[series]);
    return m_data[series][i][j];
  }
  virtual void set(double f, int series, int i, int j);

  // Counts
  int numberOfPairs() const;
  int numberOfPairsOfSeries(int series) const {
    assert(series >= 0 && series < k_numberOfSeries);
    return m_numberOfPairs[series];
  }

  // Delete and reset
  virtual void deletePairOfSeriesAtIndex(int series, int j);
  virtual void deleteAllPairsOfSeries(int series);
  void deleteAllPairs();
  void resetColumn(int series, int i);

  // Series
  virtual bool isEmpty() const;
  virtual bool seriesIsEmpty(int series) const = 0;
  virtual int numberOfNonEmptySeries() const;
  int indexOfKthNonEmptySeries(int k) const;

  // Calculations
  double sumOfColumn(int series, int i, bool lnOfSeries = false) const;
  bool seriesNumberOfAbscissaeGreaterOrEqualTo(int series, int i) const;
  uint32_t storeChecksum() const;
  uint32_t storeChecksumForSeries(int series) const;

  // Colors
  static KDColor colorOfSeriesAtIndex(int i) {
    assert(i >= 0 && i < k_numberOfSeries);
    assert(i < Palette::numberOfDataColors());
    return Palette::DataColor[i];
  }
  static KDColor colorLightOfSeriesAtIndex(int i) {
    assert(i >= 0 && i < k_numberOfSeries);
    assert(i < Palette::numberOfLightDataColors());
    return Palette::DataColorLight[i];
  }
protected:
  virtual double defaultValue(int series, int i, int j) const;
  double m_data[k_numberOfSeries][k_numberOfColumnsPerSeries][k_maxNumberOfPairs];
private:
  int m_numberOfPairs[k_numberOfSeries];
};

}

#endif
