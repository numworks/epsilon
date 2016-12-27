#include "data.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <string.h>

namespace Statistics {

Data::Data() :
  m_numberOfPairs(0),
  m_barWidth(1.0f),
  m_selectedBar(0.0f),
  m_firstBarAbscissa(0.0f),
  m_xMin(0.0f),
  m_xMax(10.0f),
  m_yMax(1.0f),
  m_xGridUnit(1.0f)
{
}

/* Raw numeric data */

int Data::numberOfPairs() const {
  return m_numberOfPairs;
}
float Data::valueAtIndex(int index) {
  assert(index < m_numberOfPairs);
  return m_values[index];
}

int Data::sizeAtIndex(int index) {
  assert(index < m_numberOfPairs);
  return m_sizes[index];
}

void Data::setValueAtIndex(float value, int index) {
  if (index >= k_maxNumberOfPairs) {
    return;
  }
  m_values[index] = value;
  if (index >= m_numberOfPairs) {
    m_sizes[index] = 1;
    m_numberOfPairs++;
  }
  initBarParameters();
  initWindowParameters();
}

void Data::setSizeAtIndex(int size, int index) {
  if (index >= k_maxNumberOfPairs) {
    return;
  }
  m_sizes[index] = size;
  if (index >= m_numberOfPairs) {
    m_values[index] = 0.0f;
    m_numberOfPairs++;
  }
  initBarParameters();
  initWindowParameters();
}

void Data::deletePairAtIndex(int index) {
  m_numberOfPairs--;
  for (int k = index; k < m_numberOfPairs; k++) {
    m_values[k] = m_values[k+1];
    m_sizes[k] = m_sizes[k+1];
  }
  m_values[m_numberOfPairs] = 0.0f;
  m_sizes[m_numberOfPairs] = 0;
  initBarParameters();
  initWindowParameters();
}

int Data::totalSize() {
  int totalSize = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    totalSize += m_sizes[k];
  }
  return totalSize;
}
/* Histogram bars */

float Data::barWidth() {
  return m_barWidth;
}

void Data::setBarWidth(float barWidth) {
  if (barWidth <= 0.0f) {
    return;
  }
  m_barWidth = barWidth;  
  initWindowParameters();
}

float Data::firsBarAbscissa() {
  return m_firstBarAbscissa;
}

void Data::setFirsBarAbscissa(float firsBarAbscissa) {
  m_firstBarAbscissa = firsBarAbscissa;
  initWindowParameters();
}

int Data::heightForBarAtValue(float value) {
  float width = barWidth();
  int barNumber = floorf((value - m_firstBarAbscissa)/width);
  float lowerBound = m_firstBarAbscissa + barNumber*width;
  float upperBound = m_firstBarAbscissa + (barNumber+1)*width;
  return sumOfValuesBetween(lowerBound, upperBound);
}

float Data::selectedBar() {
  return m_selectedBar;
}

bool Data::selectNextBarToward(int direction) {
  float newSelectedBar = m_selectedBar;
  float max = maxValue();
  float min = minValue();
  if (direction > 0.0f) {
    do {
      newSelectedBar += m_barWidth;
    } while (heightForBarAtValue(newSelectedBar) == 0 && newSelectedBar < max + m_barWidth);
  }
  if (direction < 0.0f) {
    do {
      newSelectedBar -= m_barWidth;
    } while (heightForBarAtValue(newSelectedBar) == 0 && newSelectedBar > min - m_barWidth);
  }
  if (newSelectedBar > min - m_barWidth && newSelectedBar < max + m_barWidth) {
    m_selectedBar = newSelectedBar;
    return scrollToSelectedBar();
  }
  return false;
}

/* CurveViewWindow */

float Data::xMin() {
  return m_xMin;
}

float Data::xMax() {
  return m_xMax;
}

float Data::yMin() {
  return -k_marginFactor*m_yMax;
}

float Data::yMax() {
  return m_yMax;
}

float Data::xGridUnit() {
  return m_xGridUnit;
}

float Data::maxValue() {
  float max = -FLT_MAX;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_values[k] > max && m_sizes[k] > 0) {
      max = m_values[k];
    }
  }
  return max;
}

float Data::minValue() {
  float min = FLT_MAX;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_values[k] < min && m_sizes[k] > 0) {
      min = m_values[k];
    }
  }
  return min;
}

float Data::range() {
  return maxValue()-minValue();
}

float Data::mean() {
  return sum()/(float)totalSize();
}

float Data::variance() {
  float m = mean();
  return squaredValueSum()/(float)totalSize() - m*m;
}

float Data::standardDeviation() {
  return sqrtf(variance());
}

float Data::firstQuartile() {
  int firstQuartileIndex = floorf(totalSize()/4)+1;
  return sortedElementNumber(firstQuartileIndex);
}

float Data::thirdQuartile() {
  int thirdQuartileIndex = floorf(3*totalSize()/4)+1;
  return sortedElementNumber(thirdQuartileIndex);
}

float Data::quartileRange() {
  return thirdQuartile()-firstQuartile();
}

float Data::median() {
  int total = totalSize();
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

float Data::sum() {
  float result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_values[k]*m_sizes[k];
  }
  return result;
}

float Data::squaredValueSum() {
  float result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_values[k]*m_values[k]*m_sizes[k];
  }
  return result;
}

float Data::sumOfValuesBetween(float x1, float x2) {
  int result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_values[k] < x2 && x1 <= m_values[k]) {
      result += m_sizes[k];
    }
  }
  return result;
}

bool Data::scrollToSelectedBar() {
  float range = m_xMax - m_xMin;
  if (m_xMin > m_selectedBar) {
    m_xMin = m_selectedBar - m_barWidth/2;
    m_xMax = m_xMin + range;
    return true;
  }
  if (m_selectedBar > m_xMax) {
    m_xMax = m_selectedBar + m_barWidth/2;
    m_xMin = m_xMax - range;
    return true;
  }
  return false;
}

void Data::initBarParameters() {
  float min = minValue();
  float max = maxValue();
  m_firstBarAbscissa = min;
  m_barWidth = computeGridUnit(Axis::X, min, max);
  if (m_barWidth <= 0.0f) {
    m_barWidth = 1.0f;
  }
}

void Data::initWindowParameters() {
  float min = minValue();
  float max = maxValue();
  m_xMin = m_firstBarAbscissa;
  m_xMax = max + m_barWidth;
  if ((m_xMax - m_xMin)/m_barWidth > k_maxNumberOfBarsPerWindow) {
    m_xMax = m_xMin + k_maxNumberOfBarsPerWindow*m_barWidth;
  }
  if (m_xMin >= m_xMax) {
    m_xMax = m_xMin + 10.0f*m_barWidth;
  }
  m_yMax = -FLT_MAX;
  for (float x = min; x <= max; x += m_barWidth) {
    float size = heightForBarAtValue(x);
    if (size > m_yMax) {
      m_yMax = size;
    }
  }
  m_yMax = m_yMax/(float)totalSize();
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);

  m_selectedBar = m_firstBarAbscissa + m_barWidth/2;
  while (heightForBarAtValue(m_selectedBar) == 0 && m_selectedBar < max + m_barWidth) {
    m_selectedBar += m_barWidth;
  }
  if (m_selectedBar > max + m_barWidth) {
    /* No bar is after m_firstBarAbscissa */
    m_selectedBar = m_firstBarAbscissa + m_barWidth/2;
    while (heightForBarAtValue(m_selectedBar) == 0 && m_selectedBar > min - m_barWidth) {
      m_selectedBar -= m_barWidth;
    }
  }
}

float Data::sortedElementNumber(int k) {
  // TODO: use an other algorithm (ex quickselect) to avoid quadratic complexity
  float bufferValues[m_numberOfPairs];
  memcpy(bufferValues, m_values, m_numberOfPairs*sizeof(float));
  int sortedElementIndex = 0;
  int cumulatedSize = 0;
  while (cumulatedSize < k) {
    sortedElementIndex = minIndex(bufferValues, m_numberOfPairs);
    bufferValues[sortedElementIndex] = FLT_MAX;
    cumulatedSize += m_sizes[sortedElementIndex];
  }
  return m_values[sortedElementIndex];
}

int Data::minIndex(float * bufferValues, int bufferLength) {
  int index = 0;
  for (int i = 1; i < bufferLength; i++) {
    if (bufferValues[index] > bufferValues[i]) {
      index = i;
    }
  }
  return index;
}

}
