#include "float_pair_store.h"
#include <math.h>
#include <assert.h>
#include <stddef.h>

FloatPairStore::FloatPairStore() :
  m_numberOfPairs(0)
{
}

float FloatPairStore::get(int i, int j) {
  assert(j < m_numberOfPairs);
  return m_data[i][j];
}

void FloatPairStore::set(float f, int i, int j) {
  if (j >= k_maxNumberOfPairs) {
    return;
  }
  m_data[i][j] = f;
  if (j >= m_numberOfPairs) {
    int otherI = i == 0 ? 1 : 0;
    m_data[otherI][j] = defaultValue(otherI);
    m_numberOfPairs++;
  }
}

int FloatPairStore::numberOfPairs() {
  return m_numberOfPairs;
}

void FloatPairStore::deletePairAtIndex(int i) {
  m_numberOfPairs--;
  for (int k = i; k < m_numberOfPairs; k++) {
    m_data[0][k] = m_data[0][k+1];
    m_data[1][k] = m_data[1][k+1];
  }
}

void FloatPairStore::deleteAllPairs() {
  m_numberOfPairs = 0;
}

void FloatPairStore::resetColumn(int i) {
  for (int k = 0; k < m_numberOfPairs; k++) {
    m_data[i][k] = defaultValue(i);
  }
}

float FloatPairStore::sumOfColumn(int i) {
  float result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_data[i][k];
  }
  return result;
}

uint32_t FloatPairStore::storeChecksum() {
  size_t dataLengthInBytes = m_numberOfPairs*2*sizeof(float);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  //return Ion::crc32((uint32_t *)m_data, dataLengthInBytes>>2);
  return sumOfColumn(0)+sumOfColumn(1);
}

float FloatPairStore::defaultValue(int i) {
  return 0.0f;
}
