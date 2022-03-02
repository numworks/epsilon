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
    m_numberOfValues{}
  {}
  // Delete the implicit copy constructor: the object is heavy
  DoublePairStore(const DoublePairStore&) = delete;

  // Get and set data
  double get(int series, int i, int j) const {
    assert(j < m_numberOfValues[series][i]);
    return m_data[series][i][j];
  }
  virtual void set(double f, int series, int i, int j);

  // Counts
  int numberOfPairs() const;
  int numberOfPairsOfSeries(int series) const;
  int numberOfValuesOfColumn(int series, int column) { return m_numberOfValues[series][column]; }

  // Delete and reset
  virtual void deletePairOfSeriesAtIndex(int series, int j);
  virtual void deleteAllPairsOfSeries(int series);
  void deleteColumn(int series, int column);
  void deleteAllPairs();
  void resetColumn(int series, int i);
  void makeColumnsEqualLength(int series);

  // Series
  // isEmpy, numberOfNonEmptySeries and indexOfKthNonEmptySeries treat non valid series as empty
  virtual bool isEmpty() const;
  virtual bool seriesIsValid(int series) const {
    return m_numberOfValues[series][0] == m_numberOfValues[series][1] && m_numberOfValues[series][0] > 0;
  }
  virtual int numberOfNonEmptySeries() const;
  int indexOfKthNonEmptySeries(int k) const;

  // Calculations
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
  double * data() { return reinterpret_cast<double*>(&m_data); }
protected:
  virtual double defaultValue(int series, int i, int j) const;
  double m_data[k_numberOfSeries][k_numberOfColumnsPerSeries][k_maxNumberOfPairs];
private:
  int m_numberOfValues[k_numberOfSeries][k_numberOfColumnsPerSeries];
};

}

#endif
