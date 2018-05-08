#include "float_pair_store.h"
#include <cmath>
#include <assert.h>
#include <stddef.h>
#include <ion.h>

namespace Shared {

FloatPairStore::FloatPairStore() :
  m_numberOfPairs(0),
  m_data{}
{
}

double FloatPairStore::get(int i, int j) {
  assert(j < m_numberOfPairs);
  return m_data[i][j];
}

void FloatPairStore::set(double f, int i, int j) {
  if (j >= k_maxNumberOfPairs) {
    return;
  }
  m_data[i][j] = f;
  if (j >= m_numberOfPairs) {
    int otherI = i == 0 ? 1 : 0;
    m_data[otherI][j] = defaultValue(otherI, j);
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
  /* We reset the values of the empty row to ensure the correctness of the
   * checksum. */
  m_data[0][m_numberOfPairs] = 0;
  m_data[1][m_numberOfPairs] = 0;
}

void FloatPairStore::deleteAllPairs() {
  /* We reset all values to 0 to ensure the correctness of the checksum.*/
  for (int k = 0; k < m_numberOfPairs; k++) {
    m_data[0][k] = 0;
    m_data[1][k] = 0;
  }
  m_numberOfPairs = 0;
}

void FloatPairStore::resetColumn(int i) {
  for (int k = 0; k < m_numberOfPairs; k++) {
    m_data[i][k] = defaultValue(i, k);
  }
}

double FloatPairStore::sumOfColumn(int i) {
  double result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_data[i][k];
  }
  return result;
}

uint32_t FloatPairStore::storeChecksum() {
  /* Ideally, we would only compute the checksum of the first m_numberOfPairs
   * pairs. However, the two values of a pair are not stored consecutively. We
   * thus compute the checksum on all pairs and ensure to set the pair at 0
   * when removing them. */
  size_t dataLengthInBytes = k_maxNumberOfPairs*2*sizeof(double);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  return Ion::crc32((uint32_t *)m_data, dataLengthInBytes/sizeof(uint32_t));
}

double FloatPairStore::defaultValue(int i, int j) {
  if(i == 0 && j > 1) {
    return 2*m_data[i][j-1]-m_data[i][j-2];
  } else {
    return 0.0;
  }
}

}
