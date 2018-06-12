#include "double_pair_store.h"
#include <cmath>
#include <assert.h>
#include <stddef.h>
#include <ion.h>

namespace Shared {

void DoublePairStore::set(double f, int series, int i, int j) {
  assert(series >= 0 && series < k_numberOfSeries);
  if (j >= k_maxNumberOfPairs) {
    return;
  }
  m_data[series][i][j] = f;
  if (j >= m_numberOfPairs[series]) {
    int otherI = i == 0 ? 1 : 0;
    m_data[series][otherI][j] = defaultValue(series, otherI, j);
    m_numberOfPairs[series]++;
  }
}

int DoublePairStore::numberOfPairs() const {
  int result = 0;
  for (int i = 0; i < k_numberOfSeries; i++) {
    result += m_numberOfPairs[i];
  }
  return result;
}

void DoublePairStore::deletePairOfSeriesAtIndex(int series, int j) {
  m_numberOfPairs[series]--;
  for (int k = j; k < m_numberOfPairs[series]; k++) {
    m_data[series][0][k] = m_data[series][0][k+1];
    m_data[series][1][k] = m_data[series][1][k+1];
  }
  /* We reset the values of the empty row to ensure the correctness of the
   * checksum. */
  m_data[series][0][m_numberOfPairs[series]] = 0;
  m_data[series][1][m_numberOfPairs[series]] = 0;
}

void DoublePairStore::deleteAllPairsOfSeries(int series) {
  assert(series >= 0 && series < k_numberOfSeries);
  /* We reset all values to 0 to ensure the correctness of the checksum.*/
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    m_data[series][0][k] = 0;
    m_data[series][1][k] = 0;
  }
  m_numberOfPairs[series] = 0;
}

void DoublePairStore::deleteAllPairs() {
  for (int i = 0; i < k_numberOfSeries; i ++) {
    deleteAllPairsOfSeries(i);
  }
}

void DoublePairStore::resetColumn(int series, int i) {
  assert(series >= 0 && series < k_numberOfSeries);
  assert(i == 0 || i == 1);
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    m_data[series][i][k] = defaultValue(series, i, k);
  }
}

bool DoublePairStore::isEmpty() const {
  for (int i = 0; i < k_numberOfSeries; i++) {
    if (!seriesIsEmpty(i)) {
      return false;
    }
  }
  return true;
}

int DoublePairStore::numberOfNonEmptySeries() const {
  int nonEmptySeriesCount = 0;
  for (int i = 0; i< k_numberOfSeries; i++) {
    if (!seriesIsEmpty(i)) {
      nonEmptySeriesCount++;
    }
  }
  return nonEmptySeriesCount;
}


int DoublePairStore::indexOfKthNonEmptySeries(int k) const {
  assert(k >= 0 && k < numberOfNonEmptySeries());
  int nonEmptySeriesCount = 0;
  for (int i = 0; i < k_numberOfSeries; i++) {
    if (!seriesIsEmpty(i)) {
      if (nonEmptySeriesCount == k) {
        return i;
      }
      nonEmptySeriesCount++;
    }
  }
  assert(false);
  return 0;
}

double DoublePairStore::sumOfColumn(int series, int i) const {
  assert(series >= 0 && series < k_numberOfSeries);
  assert(i == 0 || i == 1);
  double result = 0;
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    result += m_data[series][i][k];
  }
  return result;
}

uint32_t DoublePairStore::storeChecksum() const {
  /* Ideally, we would only compute the checksum of the first m_numberOfPairs
   * pairs. However, the two values of a pair are not stored consecutively. We
   * thus compute the checksum on all pairs and ensure to set the pair at 0
   * when removing them. */
  size_t dataLengthInBytes = k_numberOfSeries*k_maxNumberOfPairs*k_numberOfColumnsPerSeries*sizeof(double);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  return Ion::crc32((uint32_t *)m_data, dataLengthInBytes/sizeof(uint32_t));
}

double DoublePairStore::defaultValue(int series, int i, int j) const {
  assert(series >= 0 && series < k_numberOfSeries);
  if(i == 0 && j > 1) {
    return 2*m_data[series][i][j-1]-m_data[series][i][j-2];
  } else {
    return 0.0;
  }
}

}
