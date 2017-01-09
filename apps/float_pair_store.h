#ifndef APPS_FLOAT_PAIR_STORE_H
#define APPS_FLOAT_PAIR_STORE_H

#include <stdint.h>

class FloatPairStore {
public:
  FloatPairStore();
  // Delete the implicit copy constructor: the object is heavy
  FloatPairStore(const FloatPairStore&) = delete;
  float get(int i, int j);
  void set(float f, int i, int j);
  int numberOfPairs();
  void deletePairAtIndex(int j);
  void deleteAllPairs();
  void resetColumn(int i);
  float sumOfColumn(int i);
  uint32_t storeChecksum();
  constexpr static int k_maxNumberOfPairs = 500;
protected:
  virtual float defaultValue(int ii);
  int m_numberOfPairs;
  float m_data[2][k_maxNumberOfPairs];
};

#endif
