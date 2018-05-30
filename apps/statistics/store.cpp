#include "store.h"
#include <assert.h>
#include <float.h>
#include <cmath>
#include <string.h>
#include <ion.h>

using namespace Shared;

namespace Statistics {

static_assert(Store::k_numberOfSeries == 3, "The constructor of Statistics::Store should be changed");

Store::Store() :
  MemoizedCurveViewRange(),
  FloatPairStore(),
  m_barWidth(1.0),
  m_firstDrawnBarAbscissa(0.0),
  m_seriesEmpty{true, true, true},
  m_numberOfNonEmptySeries(0)
{
}

uint32_t Store::barChecksum() const {
  double data[2] = {m_barWidth, m_firstDrawnBarAbscissa};
  size_t dataLengthInBytes = 2*sizeof(double);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  return Ion::crc32((uint32_t *)data, dataLengthInBytes/sizeof(uint32_t));
}

/* Histogram bars */

void Store::setBarWidth(double barWidth) {
  if (barWidth > 0.0) {
    m_barWidth = barWidth;
  }
}

double Store::heightOfBarAtIndex(int series, int index) const {
  return sumOfValuesBetween(series, startOfBarAtIndex(series, index), endOfBarAtIndex(series, index));
}

double Store::heightOfBarAtValue(int series, double value) const {
  double width = barWidth();
  int barNumber = std::floor((value - m_firstDrawnBarAbscissa)/width);
  double lowerBound = m_firstDrawnBarAbscissa + barNumber*width;
  double upperBound = m_firstDrawnBarAbscissa + (barNumber+1)*width;
  return sumOfValuesBetween(series, lowerBound, upperBound);
}

double Store::startOfBarAtIndex(int series, int index) const {
  double firstBarAbscissa = m_firstDrawnBarAbscissa + m_barWidth*std::floor((minValue(series)- m_firstDrawnBarAbscissa)/m_barWidth);
  return firstBarAbscissa + index * m_barWidth;
}

double Store::endOfBarAtIndex(int series, int index) const {
  return startOfBarAtIndex(series, index+1);
}

double Store::numberOfBars(int series) const {
  double firstBarAbscissa = m_firstDrawnBarAbscissa + m_barWidth*std::floor((minValue(series)- m_firstDrawnBarAbscissa)/m_barWidth);
  return std::ceil((maxValue(series) - firstBarAbscissa)/m_barWidth)+1;
}

bool Store::scrollToSelectedBarIndex(int series, int index) {
  float startSelectedBar = startOfBarAtIndex(series, index);
  float windowRange = m_xMax - m_xMin;
  float range = windowRange/(1+k_displayLeftMarginRatio+k_displayRightMarginRatio);
  if (m_xMin + k_displayLeftMarginRatio*range > startSelectedBar) {
    m_xMin = startSelectedBar - k_displayLeftMarginRatio*range;
    m_xMax = m_xMin + windowRange;
    return true;
  }
  float endSelectedBar = endOfBarAtIndex(series, index);
  if (endSelectedBar > m_xMax - k_displayRightMarginRatio*range) {
    m_xMax = endSelectedBar + k_displayRightMarginRatio*range;
    m_xMin = m_xMax - windowRange;
    return true;
  }
  return false;
}

bool Store::isEmpty() const {
  for (int i = 0; i < k_numberOfSeries; i ++) {
    if (!seriesIsEmpty(i)) {
      return false;
    }
  }
  return true;
}

int Store::numberOfNonEmptySeries() const {
  return m_numberOfNonEmptySeries;
}

bool Store::seriesIsEmpty(int i) const {
  return m_seriesEmpty[i];
}

int Store::indexOfKthNonEmptySeries(int k) const {
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

/* Calculation */

double Store::sumOfOccurrences(int series) const {
  return sumOfColumn(series, 1);
}

double Store::maxValueForAllSeries() const {
  assert(FloatPairStore::k_numberOfSeries > 0);
  double result = maxValue(0);
  for (int i = 1; i < FloatPairStore::k_numberOfSeries; i++) {
    double maxCurrentSeries = maxValue(i);
    if (result < maxCurrentSeries) {
      result = maxCurrentSeries;
    }
  }
  return result;
}

double Store::minValueForAllSeries() const {
  assert(FloatPairStore::k_numberOfSeries > 0);
  double result = minValue(0);
  for (int i = 1; i < FloatPairStore::k_numberOfSeries; i++) {
    double minCurrentSeries = minValue(i);
    if (result > minCurrentSeries) {
      result = minCurrentSeries;
    }
  }
  return result;
}

double Store::maxValue(int series) const {
  double max = -DBL_MAX;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    if (m_data[series][0][k] > max && m_data[series][1][k] > 0) {
      max = m_data[series][0][k];
    }
  }
  return max;
}

double Store::minValue(int series) const {
  double min = DBL_MAX;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    if (m_data[series][0][k] < min && m_data[series][1][k] > 0) {
      min = m_data[series][0][k];
    }
  }
  return min;
}

double Store::range(int series) const {
  return maxValue(series)-minValue(series);
}

double Store::mean(int series) const {
  return sum(series)/sumOfOccurrences(series);
}

double Store::variance(int series) const {
  double m = mean(series);
  return squaredValueSum(series)/sumOfOccurrences(series) - m*m;
}

double Store::standardDeviation(int series) const {
  return std::sqrt(variance(series));
}

double Store::sampleStandardDeviation(int series) const {
  double n = sumOfOccurrences(series);
  double s = std::sqrt(n/(n-1.0));
  return s*standardDeviation(series);
}

double Store::firstQuartile(int series) const {
  return sortedElementAtCumulatedFrequency(series, 1.0/4.0);
}

double Store::thirdQuartile(int series) const {
  return sortedElementAtCumulatedFrequency(series, 3.0/4.0);
}

double Store::quartileRange(int series) const {
  return thirdQuartile(series)-firstQuartile(series);
}

double Store::median(int series) const {
  bool exactElement = true;
  double maxMedian = sortedElementAtCumulatedFrequency(series, 1.0/2.0, &exactElement);
  if (!exactElement) {
    double minusMedian = sortedElementAfter(series, maxMedian);
    return (minusMedian+maxMedian)/2.0;
  } else {
    return maxMedian;
  }
}

double Store::sum(int series) const {
  double result = 0;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    result += m_data[series][0][k]*m_data[series][1][k];
  }
  return result;
}

double Store::squaredValueSum(int series) const {
  double result = 0;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    result += m_data[series][0][k]*m_data[series][0][k]*m_data[series][1][k];
  }
  return result;
}

void Store::set(double f, int series, int i, int j) {
  FloatPairStore::set(f, series, i, j);
  m_seriesEmpty[series] = sumOfOccurrences(series) == 0;
  updateNonEmptySeriesCount();
}

void Store::deletePairOfSeriesAtIndex(int series, int j) {
  FloatPairStore::deletePairOfSeriesAtIndex(series, j);
  m_seriesEmpty[series] = sumOfOccurrences(series) == 0;
  updateNonEmptySeriesCount();
}

void Store::deleteAllPairsOfSeries(int series) {
  FloatPairStore::deleteAllPairsOfSeries(series);
  m_seriesEmpty[series] = true;
  updateNonEmptySeriesCount();
}

void Store::updateNonEmptySeriesCount() {
  int nonEmptySeriesCount = 0;
  for (int i = 0; i< k_numberOfSeries; i++) {
    if (!m_seriesEmpty[i]) {
      nonEmptySeriesCount++;
    }
  }
  m_numberOfNonEmptySeries = nonEmptySeriesCount;
}

/* Private methods */

double Store::defaultValue(int series, int i, int j) const {
  return i == 0 ? FloatPairStore::defaultValue(series, i, j) : 1.0;
}

double Store::sumOfValuesBetween(int series, double x1, double x2) const {
  double result = 0;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    if (m_data[series][0][k] < x2 && x1 <= m_data[series][0][k]) {
      result += m_data[series][1][k];
    }
  }
  return result;
}

double Store::sortedElementAtCumulatedFrequency(int series, double k, bool * exactElement) const {
  // TODO: use an other algorithm (ex quickselect) to avoid quadratic complexity
  assert(k >= 0.0 && k <= 1.0);
  double totalNumberOfElements = sumOfOccurrences(series);
  double bufferValues[numberOfPairsOfSeries(series)];
  memcpy(bufferValues, m_data[series][0], numberOfPairsOfSeries(series)*sizeof(double));
  int sortedElementIndex = 0;
  double cumulatedFrequency = 0.0;
  while (cumulatedFrequency < k) {
    sortedElementIndex = minIndex(bufferValues, numberOfPairsOfSeries(series));
    bufferValues[sortedElementIndex] = DBL_MAX;
    cumulatedFrequency += m_data[series][1][sortedElementIndex] / totalNumberOfElements;
    if (exactElement != nullptr && cumulatedFrequency == k) {
      *exactElement = false;
    }
  }
  return m_data[series][0][sortedElementIndex];
}

double Store::sortedElementAfter(int series, double k) const {
  assert(numberOfPairsOfSeries(series) > 0);
  double result = DBL_MAX;
  for (int i = 0; i < numberOfPairsOfSeries(series); i++) {
    double currentElement = m_data[series][0][i];
    if (currentElement > k && currentElement < result) {
      result = currentElement;
    }
  }
  assert(result < DBL_MAX);
  return result;
}

int Store::minIndex(double * bufferValues, int bufferLength) const {
  int index = 0;
  for (int i = 1; i < bufferLength; i++) {
    if (bufferValues[index] > bufferValues[i]) {
      index = i;
    }
  }
  return index;
}

}
