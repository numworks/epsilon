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

double Store::barWidth() {
  return m_barWidth;
}

void Store::setBarWidth(double barWidth) {
  if (barWidth <= 0.0) {
    return;
  }
  m_barWidth = barWidth;
}

double Store::firstDrawnBarAbscissa() {
  return m_firstDrawnBarAbscissa;
}

void Store::setFirstDrawnBarAbscissa(double firstBarAbscissa) {
  m_firstDrawnBarAbscissa = firstBarAbscissa;
}

double Store::heightOfBarAtIndex(int index) {
  return sumOfValuesBetween(startOfBarAtIndex(index), endOfBarAtIndex(index));
}

double Store::heightOfBarAtValue(double value) {
  double width = barWidth();
  int barNumber = std::floor((value - m_firstDrawnBarAbscissa)/width);
  double lowerBound = m_firstDrawnBarAbscissa + barNumber*width;
  double upperBound = m_firstDrawnBarAbscissa + (barNumber+1)*width;
  return sumOfValuesBetween(lowerBound, upperBound);
}

double Store::startOfBarAtIndex(int index) {
  double firstBarAbscissa = m_firstDrawnBarAbscissa + m_barWidth*std::floor((minValue()- m_firstDrawnBarAbscissa)/m_barWidth);
  return firstBarAbscissa + index * m_barWidth;
}

double Store::endOfBarAtIndex(int index) {
  return startOfBarAtIndex(index+1);
}

double Store::numberOfBars() {
  double firstBarAbscissa = m_firstDrawnBarAbscissa + m_barWidth*std::floor((minValue()- m_firstDrawnBarAbscissa)/m_barWidth);
  return std::ceil((maxValue() - firstBarAbscissa)/m_barWidth)+1;
}

bool Store::scrollToSelectedBarIndex(int index) {
  float startSelectedBar = startOfBarAtIndex(index);
  float windowRange = m_xMax - m_xMin;
  float range = windowRange/(1+k_displayLeftMarginRatio+k_displayRightMarginRatio);
  if (m_xMin + k_displayLeftMarginRatio*range > startSelectedBar) {
    m_xMin = startSelectedBar - k_displayLeftMarginRatio*range;
    m_xMax = m_xMin + windowRange;
    return true;
  }
  float endSelectedBar = endOfBarAtIndex(index);
  if (endSelectedBar > m_xMax - k_displayRightMarginRatio*range) {
    m_xMax = endSelectedBar + k_displayRightMarginRatio*range;
    m_xMin = m_xMax - windowRange;
    return true;
  }
  return false;
}

/* Calculation */

double Store::sumOfOccurrences() {
  return sumOfColumn(1);
}

double Store::maxValue() {
  double max = -DBL_MAX;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_data[0][k] > max && m_data[1][k] > 0) {
      max = m_data[0][k];
    }
  }
  return max;
}

double Store::minValue() {
  double min = DBL_MAX;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_data[0][k] < min && m_data[1][k] > 0) {
      min = m_data[0][k];
    }
  }
  return min;
}

double Store::range() {
  return maxValue()-minValue();
}

double Store::mean() {
  return sum()/sumOfColumn(1);
}

double Store::variance() {
  double m = mean();
  return squaredValueSum()/sumOfColumn(1) - m*m;
}

double Store::standardDeviation() {
  return std::sqrt(variance());
}

double Store::sampleStandardDeviation() {
  double n = sumOfColumn(1);
  double s = std::sqrt(n/(n-1.0));
  return s*standardDeviation();
}

double Store::firstQuartile() {
  int firstQuartileIndex = std::ceil(sumOfColumn(1)/4);
  return sortedElementNumber(firstQuartileIndex);
}

double Store::thirdQuartile() {
  int thirdQuartileIndex = std::ceil(3*sumOfColumn(1)/4);
  return sortedElementNumber(thirdQuartileIndex);
}

double Store::quartileRange() {
  return thirdQuartile()-firstQuartile();
}

double Store::median() {
  int total = sumOfColumn(1);
  int halfTotal = total/2;
  int totalMod2 = total - 2*halfTotal;
  if (totalMod2 == 0) {
    double minusMedian = sortedElementNumber(halfTotal);
    double maxMedian = sortedElementNumber(halfTotal+1);
    return (minusMedian+maxMedian)/2.0;
  } else {
    return sortedElementNumber(halfTotal+1);
  }
}

double Store::sum() {
  double result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_data[0][k]*m_data[1][k];
  }
  return result;
}

double Store::squaredValueSum() {
  double result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_data[0][k]*m_data[0][k]*m_data[1][k];
  }
  return result;
}

/* private methods */

double Store::defaultValue(int i, int j) {
  if (i == 0) {
    return FloatPairStore::defaultValue(i, j);
  } else {
    return 1.0;
  }
}

double Store::sumOfValuesBetween(double x1, double x2) {
  int result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_data[0][k] < x2 && x1 <= m_data[0][k]) {
      result += m_data[1][k];
    }
  }
  return result;
}

double Store::sortedElementNumber(int k) {
  // TODO: use an other algorithm (ex quickselect) to avoid quadratic complexity
  double bufferValues[m_numberOfPairs];
  memcpy(bufferValues, m_data[0], m_numberOfPairs*sizeof(double));
  int sortedElementIndex = 0;
  double cumulatedSize = 0.0;
  while (cumulatedSize < k) {
    sortedElementIndex = minIndex(bufferValues, m_numberOfPairs);
    bufferValues[sortedElementIndex] = DBL_MAX;
    cumulatedSize += m_data[1][sortedElementIndex];
  }
  return m_data[0][sortedElementIndex];
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
