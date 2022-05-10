#include "double_pair_store.h"
#include <poincare/helpers.h>
#include <cmath>
#include <assert.h>
#include <stddef.h>
#include <ion.h>
#include <algorithm>

namespace Shared {

void DoublePairStore::set(double f, int series, int i, int j) {
  assert(series >= 0 && series < k_numberOfSeries);
  if (j >= k_maxNumberOfPairs) {
    return;
  }
  assert(j <= m_numberOfPairs[series]);
  m_data[series][i][j] = f;
  if (j >= m_numberOfPairs[series]) {
    int otherI = i == 0 ? 1 : 0;
    m_data[series][otherI][j] = defaultValue(series, otherI, j);
    assert(m_numberOfPairs[series] < UINT8_MAX);
    m_numberOfPairs[series]++;
  }
  updateSeriesValidity(series);
}

int DoublePairStore::numberOfPairs() const {
  int result = 0;
  for (int i = 0; i < k_numberOfSeries; i++) {
    result += m_numberOfPairs[i];
  }
  return result;
}

bool DoublePairStore::deleteValueAtIndex(int series, int i, int j) {
  assert(series >= 0 && series < k_numberOfSeries);
  assert(j >= 0 && j < m_numberOfPairs[series]);
  int otherI = i == 0 ? 1 : 0;
  if (std::isnan(m_data[series][otherI][j])) {
    deletePairOfSeriesAtIndex(series, j);
    return true;
  } else {
    m_data[series][i][j] = NAN;
    updateSeriesValidity(series);
    return false;
  }
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
  updateSeriesValidity(series);
}

void DoublePairStore::deleteAllPairsOfSeries(int series) {
  assert(series >= 0 && series < k_numberOfSeries);
  /* We reset all values to 0 to ensure the correctness of the checksum.*/
  for (int i = 0 ; i < k_numberOfColumnsPerSeries ; i++) {
    for (int k = 0 ; k < m_numberOfPairs[series] ; k++) {
      m_data[series][i][k] = 0.0;
    }
  }
  m_numberOfPairs[series] = 0;
  updateSeriesValidity(series);
}

void DoublePairStore::deleteAllPairs() {
  for (int i = 0; i < k_numberOfSeries; i ++) {
    deleteAllPairsOfSeries(i);
  }
}

void DoublePairStore::deleteColumn(int series, int i) {
  assert(series >= 0 && series < k_numberOfSeries);
  assert(i == 0 || i == 1);
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    if(deleteValueAtIndex(series, i, k)) {
      k--;
    }
  }
  updateSeriesValidity(series);
}

void DoublePairStore::resetColumn(int series, int i) {
  assert(series >= 0 && series < k_numberOfSeries);
  assert(i == 0 || i == 1);
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    m_data[series][i][k] = defaultValue(series, i, k);
  }
  updateSeriesValidity(series);
}

bool DoublePairStore::hasValidSeries(ValidSeries validSeries) const {
  for (int i = 0; i < k_numberOfSeries; i++) {
    if (validSeries(this, i)) {
      return true;
    }
  }
  return false;
}

bool DoublePairStore::seriesIsValid(int series) const {
  assert(series >= 0 && series < k_numberOfSeries);
  return m_validSeries[series];
}

void DoublePairStore::updateSeriesValidity(int series) {
  assert(series >= 0 && series < k_numberOfSeries);
  if (m_numberOfPairs[series] == 0) {
    m_validSeries[series] = false;
    return;
  }
  for (int i = 0 ; i < k_numberOfColumnsPerSeries ; i++) {
    for (int j = 0 ; j < m_numberOfPairs[series] ; j ++) {
      if (std::isnan(m_data[series][i][j])) {
        m_validSeries[series] = false;
        return;
      }
    }
  }
  m_validSeries[series] = true;
}

int DoublePairStore::numberOfValidSeries(ValidSeries validSeries) const {
  int nonEmptySeriesCount = 0;
  for (int i = 0; i< k_numberOfSeries; i++) {
    if (validSeries(this, i)) {
      nonEmptySeriesCount++;
    }
  }
  return nonEmptySeriesCount;
}


int DoublePairStore::indexOfKthValidSeries(int k, ValidSeries validSeries) const {
  assert(k >= 0 && k < numberOfValidSeries());
  int validSeriesCount = 0;
  for (int i = 0; i < k_numberOfSeries; i++) {
    if (validSeries(this, i)) {
      if (validSeriesCount == k) {
        return i;
      }
      validSeriesCount++;
    }
  }
  assert(false);
  return 0;
}

void DoublePairStore::sortColumn(int series, int column) {
  assert(column == 0 || column == 1);
  static Poincare::Helpers::Swap swapRows = [](int i, int j, void * context, int numberOfElements) {
    // Swap X and Y values
    double * dataX = static_cast<double*>(context);
    double * dataY = static_cast<double*>(context) + DoublePairStore::k_maxNumberOfPairs;
    double tempX = dataX[i];
    double tempY = dataY[i];
    dataX[i] = dataX[j];
    dataY[i] = dataY[j];
    dataX[j] = tempX;
    dataY[j] = tempY;
  };
  static Poincare::Helpers::Compare compareX = [](int a, int b, void * context, int numberOfElements)->bool{
    double * dataX = static_cast<double*>(context);
    return dataX[a] >= dataX[b] || std::isnan(dataX[a]);
  };
  static Poincare::Helpers::Compare compareY = [](int a, int b, void * context, int numberOfElements)->bool{
    double * dataY = static_cast<double*>(context) + DoublePairStore::k_maxNumberOfPairs;
    return dataY[a] >= dataY[b] || std::isnan(dataY[a]);
  };
  int indexOfFirstCell = series * DoublePairStore::k_numberOfColumnsPerSeries * DoublePairStore::k_maxNumberOfPairs;
  double * seriesContext = &(data()[indexOfFirstCell]);
  Poincare::Helpers::Sort(swapRows, (column == 0) ? compareX : compareY, seriesContext, numberOfPairsOfSeries(series));
}

void DoublePairStore::sortIndexByColumn(uint8_t * sortedIndex, int series, int column, int startIndex, int endIndex) const {
  assert(startIndex < endIndex);
  void * pack[] = { const_cast<DoublePairStore *>(this), sortedIndex + startIndex, &series, &column };
  Poincare::Helpers::Sort(
      [](int i, int j, void * ctx, int n) { // Swap method
        void ** pack = reinterpret_cast<void **>(ctx);
        uint8_t * sortedIndex = reinterpret_cast<uint8_t *>(pack[1]);
        uint8_t t = sortedIndex[i];
        sortedIndex[i] = sortedIndex[j];
        sortedIndex[j] = t;
      },
      [](int i, int j, void * ctx, int n) { // Comparison method
        void ** pack = reinterpret_cast<void **>(ctx);
        const DoublePairStore * store = reinterpret_cast<const DoublePairStore *>(pack[0]);
        uint8_t * sortedIndex = reinterpret_cast<uint8_t *>(pack[1]);
        int series = *reinterpret_cast<int *>(pack[2]);
        int column = *reinterpret_cast<int *>(pack[3]);
        return store->get(series, column, sortedIndex[i]) >= store->get(series, column, sortedIndex[j]);
      }, pack, endIndex - startIndex);
}

double DoublePairStore::sumOfColumn(int series, int i, bool lnOfSeries) const {
  assert(series >= 0 && series < k_numberOfSeries);
  assert(i == 0 || i == 1);
  double result = 0;
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    result += lnOfSeries ? log(m_data[series][i][k]) : m_data[series][i][k];
  }
  return result;
}

bool DoublePairStore::seriesNumberOfAbscissaeGreaterOrEqualTo(int series, int i) const {
  assert(series >= 0 && series < k_numberOfSeries);
  int count = 0;
  for (int j = 0; j < m_numberOfPairs[series]; j++) {
    if (count >= i) {
      return true;
    }
    double currentAbsissa = m_data[series][0][j];
    bool firstOccurence = true;
    for (int k = 0; k < j; k++) {
      if (m_data[series][0][k] == currentAbsissa) {
        firstOccurence = false;
        break;
      }
    }
    if (firstOccurence) {
      count++;
    }
  }
  return count >= i;
}

uint32_t DoublePairStore::storeChecksum() const {
  uint32_t checkSumPerSeries[k_numberOfSeries];
  for (int i = 0; i < k_numberOfSeries; i++) {
    checkSumPerSeries[i] = storeChecksumForSeries(i);
  }
  return Ion::crc32Word(checkSumPerSeries, k_numberOfSeries);
}

uint32_t DoublePairStore::storeChecksumForSeries(int series) const {
  /* Ideally, we would compute the checksum of the first m_numberOfPairs pairs.
   * However, the two values of a pair are not stored consecutively. We thus
   * compute the checksum of the x values of the pairs, then we compute the
   * checksum of the y values of the pairs, and finally we compute the checksum
   * of the checksums.
   * We cannot simply put "empty" values to 0 and compute the checksum of the
   * whole data, because adding or removing (0, 0) "real" data pairs would not
   * change the checksum. */
  size_t dataLengthInBytesPerDataColumn = m_numberOfPairs[series]*sizeof(double);
  assert((dataLengthInBytesPerDataColumn & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  uint32_t checkSumPerColumn[k_numberOfColumnsPerSeries];
  for (int i = 0; i < k_numberOfColumnsPerSeries; i++) {
    checkSumPerColumn[i] = Ion::crc32Word((uint32_t *)m_data[series][i], dataLengthInBytesPerDataColumn/sizeof(uint32_t));
  }
  return Ion::crc32Word(checkSumPerColumn, k_numberOfColumnsPerSeries);
}

double DoublePairStore::defaultValue(int series, int i, int j) const {
  assert(series >= 0 && series < k_numberOfSeries);
  return 0.0;
}

}
