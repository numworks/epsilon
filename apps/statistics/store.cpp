#include "store.h"
#include <assert.h>
#include <float.h>
#include <cmath>
#include <string.h>
#include <ion.h>

using namespace Shared;

namespace Statistics {

Store::Store() :
  MemoizedCurveViewRange(),
  FloatPairStore(),
  m_barWidth(1.0),
  m_firstDrawnBarAbscissa(0.0)
{
}

uint32_t Store::barChecksum() {
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

double Store::heightOfBarAtIndex(int series, int index) {
  return sumOfValuesBetween(series, startOfBarAtIndex(series, index), endOfBarAtIndex(series, index));
}

double Store::heightOfBarAtValue(int series, double value) {
  double width = barWidth();
  int barNumber = std::floor((value - m_firstDrawnBarAbscissa)/width);
  double lowerBound = m_firstDrawnBarAbscissa + barNumber*width;
  double upperBound = m_firstDrawnBarAbscissa + (barNumber+1)*width;
  return sumOfValuesBetween(series, lowerBound, upperBound);
}

double Store::startOfBarAtIndex(int series, int index) {
  double firstBarAbscissa = m_firstDrawnBarAbscissa + m_barWidth*std::floor((minValue(series)- m_firstDrawnBarAbscissa)/m_barWidth);
  return firstBarAbscissa + index * m_barWidth;
}

double Store::endOfBarAtIndex(int series, int index) {
  return startOfBarAtIndex(series, index+1);
}

double Store::numberOfBars(int series) {
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

bool Store::isEmpty() {
  for (int i = 0; i < k_numberOfSeries; i ++) {
    if (!seriesIsEmpty(i)) {
      return false;
    }
  }
  return true;
}

int Store::numberOfNonEmptySeries() {
  int result = 0;
  for (int i = 0; i < k_numberOfSeries; i ++) {
    if (!seriesIsEmpty(i)) {
      result++;
    }
  }
  return result;
}

bool Store::seriesIsEmpty(int i) {
  return sumOfOccurrences(i) == 0;
}


/* Calculation */

double Store::sumOfOccurrences(int series) {
  return sumOfColumn(series, 1);
}

double Store::maxValueForAllSeries() {
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

double Store::minValueForAllSeries() {
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

double Store::maxValue(int series) {
  double max = -DBL_MAX;
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    if (m_data[series][0][k] > max && m_data[series][1][k] > 0) {
      max = m_data[series][0][k];
    }
  }
  return max;
}

double Store::minValue(int series) {
  double min = DBL_MAX;
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    if (m_data[series][0][k] < min && m_data[series][1][k] > 0) {
      min = m_data[series][0][k];
    }
  }
  return min;
}

double Store::range(int series) {
  return maxValue(series)-minValue(series);
}

double Store::mean(int series) {
  return sum(series)/sumOfOccurrences(series);
}

double Store::variance(int series) {
  double m = mean(series);
  return squaredValueSum(series)/sumOfOccurrences(series) - m*m;
}

double Store::standardDeviation(int series) {
  return std::sqrt(variance(series));
}

double Store::sampleStandardDeviation(int series) {
  double n = sumOfOccurrences(series);
  double s = std::sqrt(n/(n-1.0));
  return s*standardDeviation(series);
}

double Store::firstQuartile(int series) {
  double firstQuartileIndex = sumOfOccurrences(series)/4.0f;
  return sortedElementNumber(series, firstQuartileIndex);
}

double Store::thirdQuartile(int series) {
  double thirdQuartileIndex = 3*sumOfOccurrences(series)/4.0f;
  return sortedElementNumber(series, thirdQuartileIndex);
}

double Store::quartileRange(int series) {
  return thirdQuartile(series)-firstQuartile(series);
}

double Store::median(int series) {
  double total = sumOfOccurrences(series);
  double halfTotal = total/2;
  int totalMod2 = total - 2*halfTotal;
  if (totalMod2 == 0) {
    double minusMedian = sortedElementNumber(series, halfTotal);
    double maxMedian = sortedElementNumber(series, halfTotal+1.0);
    return (minusMedian+maxMedian)/2.0;
  } else {
    return sortedElementNumber(series, halfTotal+1.0);
  }
}

double Store::sum(int series) {
  double result = 0;
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    result += m_data[series][0][k]*m_data[series][1][k];
  }
  return result;
}

double Store::squaredValueSum(int series) {
  double result = 0;
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    result += m_data[series][0][k]*m_data[series][0][k]*m_data[series][1][k];
  }
  return result;
}

/* Private methods */

double Store::defaultValue(int series, int i, int j) {
  return i == 0 ? FloatPairStore::defaultValue(series, i, j) : 1.0;
}

double Store::sumOfValuesBetween(int series, double x1, double x2) {
  double result = 0;
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    if (m_data[series][0][k] < x2 && x1 <= m_data[series][0][k]) {
      result += m_data[series][1][k];
    }
  }
  return result;
}

double Store::sortedElementNumber(int series, double k) {
  // TODO: use an other algorithm (ex quickselect) to avoid quadratic complexity
  double bufferValues[m_numberOfPairs[series]];
  memcpy(bufferValues, m_data[series][0], m_numberOfPairs[series]*sizeof(double));
  int sortedElementIndex = 0;
  double cumulatedSize = 0.0;
  while (cumulatedSize < k) {
    sortedElementIndex = minIndex(bufferValues, m_numberOfPairs[series]);
    bufferValues[sortedElementIndex] = DBL_MAX;
    cumulatedSize += m_data[series][1][sortedElementIndex];
  }
  return m_data[series][0][sortedElementIndex];
}

int Store::minIndex(double * bufferValues, int bufferLength) {
  int index = 0;
  for (int i = 1; i < bufferLength; i++) {
    if (bufferValues[index] > bufferValues[i]) {
      index = i;
    }
  }
  return index;
}

}
