#ifndef SHARED_FLOAT_PAIR_STORE_H
#define SHARED_FLOAT_PAIR_STORE_H

#include <stdint.h>
#include <assert.h>

namespace Shared {

class FloatPairStore {
public:
  constexpr static int k_numberOfSeries = 3;
  constexpr static int k_maxNumberOfPairs = 100;
  FloatPairStore() :
    m_numberOfPairs{},
    m_data{}
  {}
  // Delete the implicit copy constructor: the object is heavy
  FloatPairStore(const FloatPairStore&) = delete;
  double get(int series, int i, int j) const {
    assert(j < m_numberOfPairs[series]);
    return m_data[series][i][j];
  }
  void set(double f, int series, int i, int j);
  int numberOfPairsOfAllSeries() const;
  int numberOfPairs(int series) const {
    assert(series >= 0 && series < k_numberOfSeries);
    return m_numberOfPairs[series];
  }
  void deletePairAtIndex(int series, int j);
  void deleteAllPairs(int series);
  void deleteAllPairsOfAllSeries();
  void resetColumn(int series, int i);
  double sumOfColumn(int series, int i) const;
  uint32_t storeChecksum();
protected:
  virtual double defaultValue(int series, int i, int j);
  int m_numberOfPairs[k_numberOfSeries];
  double m_data[k_numberOfSeries][2][k_maxNumberOfPairs];
};

}

#endif
