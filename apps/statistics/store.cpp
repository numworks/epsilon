#include "store.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <string.h>

namespace Statistics {

Store::Store() :
  FloatPairStore(),
  m_barWidth(1.0f),
  m_selectedBar(0.0f),
  m_firstBarAbscissa(0.0f),
  m_xMin(0.0f),
  m_xMax(10.0f),
  m_yMax(1.0f),
  m_xGridUnit(1.0f)
{
}

/* Histogram bars */

void Store::initBarParameters() {
  float min = minValue();
  float max = maxValue();
  m_firstBarAbscissa = min;
  m_barWidth = computeGridUnit(Axis::X, min, max);
  if (m_barWidth <= 0.0f) {
    m_barWidth = 1.0f;
  }
}

float Store::barWidth() {
  return m_barWidth;
}

void Store::setBarWidth(float barWidth) {
  if (barWidth <= 0.0f) {
    return;
  }
  m_barWidth = barWidth;  
}

float Store::firsBarAbscissa() {
  return m_firstBarAbscissa;
}

void Store::setFirsBarAbscissa(float firsBarAbscissa) {
  m_firstBarAbscissa = firsBarAbscissa;
}

int Store::heightForBarAtValue(float value) {
  float width = barWidth();
  int barNumber = floorf((value - m_firstBarAbscissa)/width);
  float lowerBound = m_firstBarAbscissa + barNumber*width;
  float upperBound = m_firstBarAbscissa + (barNumber+1)*width;
  return sumOfValuesBetween(lowerBound, upperBound);
}

float Store::selectedBar() {
  return m_selectedBar;
}

bool Store::selectNextBarToward(int direction) {
  float newSelectedBar = m_selectedBar;
  float max = maxValue();
  float min = minValue();
  if (direction > 0.0f) {
    do {
      newSelectedBar += m_barWidth;
      newSelectedBar = closestMiddleBarTo(newSelectedBar);
    } while (heightForBarAtValue(newSelectedBar) == 0 && newSelectedBar < max + m_barWidth);
  }
  if (direction < 0.0f) {
    do {
      newSelectedBar -= m_barWidth;
      newSelectedBar = closestMiddleBarTo(newSelectedBar);
    } while (heightForBarAtValue(newSelectedBar) == 0 && newSelectedBar > min - m_barWidth);
  }
  if (newSelectedBar > min - m_barWidth && newSelectedBar < max + m_barWidth) {
    m_selectedBar = newSelectedBar;
    return scrollToSelectedBar();
  }
  return false;
}

/* CurveViewWindow */

void Store::initWindowParameters() {
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
  for (float x = min+m_barWidth/2.0f; x < max+m_barWidth; x += m_barWidth) {
    float size = heightForBarAtValue(x);
    if (size > m_yMax) {
      m_yMax = size;
    }
  }
  m_yMax = m_yMax/sumOfColumn(1);
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);

  m_selectedBar = m_firstBarAbscissa + m_barWidth/2.0f;
  while (heightForBarAtValue(m_selectedBar) == 0 && m_selectedBar < max + m_barWidth) {
    m_selectedBar += m_barWidth;
    m_selectedBar = closestMiddleBarTo(m_selectedBar);
  }
  if (m_selectedBar > max + m_barWidth) {
    /* No bar is after m_firstBarAbscissa */
    m_selectedBar = m_firstBarAbscissa + m_barWidth/2.0f;
    while (heightForBarAtValue(m_selectedBar) == 0 && m_selectedBar > min - m_barWidth) {
      m_selectedBar -= m_barWidth;
      m_selectedBar = closestMiddleBarTo(m_selectedBar);
    }
  }
}

float Store::xMin() {
  return m_xMin;
}

float Store::xMax() {
  return m_xMax;
}

float Store::yMin() {
  return 0.0f;
}

float Store::yMax() {
  return m_yMax;
}

float Store::xGridUnit() {
  return m_xGridUnit;
}
  
/* Calculation */

float Store::maxValue() {
  float max = -FLT_MAX;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_data[0][k] > max && m_data[1][k] > 0) {
      max = m_data[0][k];
    }
  }
  return max;
}

float Store::minValue() {
  float min = FLT_MAX;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_data[0][k] < min && m_data[1][k] > 0) {
      min = m_data[0][k];
    }
  }
  return min;
}

float Store::range() {
  return maxValue()-minValue();
}

float Store::mean() {
  return sum()/sumOfColumn(1);
}

float Store::variance() {
  float m = mean();
  return squaredValueSum()/sumOfColumn(1) - m*m;
}

float Store::standardDeviation() {
  return sqrtf(variance());
}

float Store::firstQuartile() {
  int firstQuartileIndex = floorf(sumOfColumn(1)/4)+1;
  return sortedElementNumber(firstQuartileIndex);
}

float Store::thirdQuartile() {
  int thirdQuartileIndex = floorf(3*sumOfColumn(1)/4)+1;
  return sortedElementNumber(thirdQuartileIndex);
}

float Store::quartileRange() {
  return thirdQuartile()-firstQuartile();
}

float Store::median() {
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

float Store::sum() {
  float result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_data[0][k]*m_data[1][k];
  }
  return result;
}

float Store::squaredValueSum() {
  float result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_data[0][k]*m_data[0][k]*m_data[1][k];
  }
  return result;
}

/* private methods */

float Store::defaultValue(int i) {
  if (i == 0) {
    return 0.0f;
  }
  return 1.0f;
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

bool Store::scrollToSelectedBar() {
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

float Store::sortedElementNumber(int k) {
  // TODO: use an other algorithm (ex quickselect) to avoid quadratic complexity
  float bufferValues[m_numberOfPairs];
  memcpy(bufferValues, m_data[0], m_numberOfPairs*sizeof(float));
  int sortedElementIndex = 0;
  int cumulatedSize = 0;
  while (cumulatedSize < k) {
    sortedElementIndex = minIndex(bufferValues, m_numberOfPairs);
    bufferValues[sortedElementIndex] = FLT_MAX;
    cumulatedSize += m_data[1][sortedElementIndex];
  }
  return m_data[0][sortedElementIndex];
}

int Store::minIndex(float * bufferValues, int bufferLength) {
  int index = 0;
  for (int i = 1; i < bufferLength; i++) {
    if (bufferValues[index] > bufferValues[i]) {
      index = i;
    }
  }
  return index;
}

float Store::closestMiddleBarTo(float f) {
  return m_firstBarAbscissa + roundf((f-m_firstBarAbscissa - m_barWidth/2.0f) / m_barWidth) * m_barWidth + m_barWidth/2.0f;
}

}
