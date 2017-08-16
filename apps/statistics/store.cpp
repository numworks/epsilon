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
  m_barWidth(1.0f),
  m_firstDrawnBarAbscissa(0.0f)
{
}

uint32_t Store::barChecksum() {
  float data[2] = {m_barWidth, m_firstDrawnBarAbscissa};
  size_t dataLengthInBytes = 2*sizeof(float);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  return Ion::crc32((uint32_t *)data, dataLengthInBytes>>2);
}

/* Histogram bars */

float Store::barWidth() {
  return m_barWidth;
}

void Store::setBarWidth(float barWidth) {
  if (barWidth <= 0.0f) {
    return;
  }
  m_barWidth = barWidth;
}

float Store::firstDrawnBarAbscissa() {
  return m_firstDrawnBarAbscissa;
}

void Store::setFirstDrawnBarAbscissa(float firstBarAbscissa) {
  m_firstDrawnBarAbscissa = firstBarAbscissa;
}

float Store::heightOfBarAtIndex(int index) {
  return sumOfValuesBetween(startOfBarAtIndex(index), endOfBarAtIndex(index));
}

float Store::heightOfBarAtValue(float value) {
  float width = barWidth();
  int barNumber = std::floor((value - m_firstDrawnBarAbscissa)/width);
  float lowerBound = m_firstDrawnBarAbscissa + barNumber*width;
  float upperBound = m_firstDrawnBarAbscissa + (barNumber+1)*width;
  return sumOfValuesBetween(lowerBound, upperBound);
}

float Store::startOfBarAtIndex(int index) {
  float firstBarAbscissa = m_firstDrawnBarAbscissa + m_barWidth*std::floor((minValue()- m_firstDrawnBarAbscissa)/m_barWidth);
  return firstBarAbscissa + index * m_barWidth;
}

float Store::endOfBarAtIndex(int index) {
  return startOfBarAtIndex(index+1);
}

int Store::numberOfBars() {
  float firstBarAbscissa = m_firstDrawnBarAbscissa + m_barWidth*std::floor((minValue()- m_firstDrawnBarAbscissa)/m_barWidth);
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
  float m = mean();
  return squaredValueSum()/sumOfColumn(1) - m*m;
}

double Store::standardDeviation() {
  return std::sqrt(variance());
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
    float minusMedian = sortedElementNumber(halfTotal);
    float maxMedian = sortedElementNumber(halfTotal+1);
    return (minusMedian+maxMedian)/2.0f;
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
  float result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_data[0][k]*m_data[0][k]*m_data[1][k];
  }
  return result;
}

/* private methods */

double Store::defaultValue(int i) {
  if (i == 0) {
    return 0.0;
  }
  return 1.0;
}

float Store::sumOfValuesBetween(float x1, float x2) {
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
