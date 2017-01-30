#include "store.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <string.h>

namespace Regression {

Store::Store() :
  InteractiveCurveViewRange(nullptr, this),
  FloatPairStore()
{
}

bool Store::didChangeRange(InteractiveCurveViewRange * interactiveCurveViewRange) {
  if (!m_yAuto) {
    return false;
  }
  float min = m_yMin;
  float max = m_yMax;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_xMin <= m_data[0][k] && m_data[0][k] <= m_xMax) {
      if (m_data[1][k] < min) {
        min = m_data[1][k];
      }
      if (m_data[1][k] > max) {
        max = m_data[1][k];
      }
    }
  }
  if (min == m_yMin && max == m_yMax) {
    return false;
  }
  m_yMin = min;
  m_yMax = max;
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  return true;
}

/* Dots */

int Store::closestVerticalDot(int direction, float x) {
  float nextX = INFINITY;
  float nextY = INFINITY;
  int selectedDot = -1;
  /* The conditions to test on all dots are in this order:
  * - the next dot should be within the window abscissa bounds
  * - the next dot is the closest one in abscissa to x
  * - the next dot is above the regression curve if direction == 1 and below
  * otherwise */
  for (int index = 0; index < m_numberOfPairs; index++) {
    if ((m_xMin <= m_data[0][index] && m_data[0][index] <= m_xMax) &&
        (fabsf(m_data[0][index] - x) < fabsf(nextX - x)) &&
        ((m_data[1][index] - yValueForXValue(m_data[0][index]) >= 0) == (direction > 0))) {
      // Handle edge case: if 2 dots have the same abscissa but different ordinates
      if (nextX != m_data[0][index] || ((nextY - m_data[1][index] >= 0) == (direction > 0))) {
        nextX = m_data[0][index];
        nextY = m_data[1][index];
        selectedDot = index;
      }
    }
  }
  // Compare with the mean dot
  if (m_xMin <= meanOfColumn(0) && meanOfColumn(0) <= m_xMax &&
      (fabsf(meanOfColumn(0) - x) < fabsf(nextX - x)) &&
      ((meanOfColumn(1) - yValueForXValue(meanOfColumn(0)) >= 0) == (direction > 0))) { 
    if (nextX != meanOfColumn(0) || ((nextY - meanOfColumn(1) >= 0) == (direction > 0))) {
      nextX = meanOfColumn(0);
      nextY = meanOfColumn(1);
      selectedDot = m_numberOfPairs;
    }
  }
  return selectedDot;
}

int Store::nextDot(int direction, int dot) {
  float nextX = INFINITY;
  int selectedDot = -1;
  float x = meanOfColumn(0);
  if (dot >= 0 && dot < m_numberOfPairs) {
    x = get(0, dot);
  }
  /* We have to scan the Store in opposite ways for the 2 directions to ensure to
   * select all dots (even with equal abscissa) */
  if (direction > 0) {
    for (int index = 0; index < m_numberOfPairs; index++) {
      /* The conditions to test are in this order:
       * - the next dot is the closest one in abscissa to x
       * - the next dot is not the same as the selected one
       * - the next dot is at the right of the selected one */
      if (fabsf(m_data[0][index] - x) < fabsf(nextX - x) &&
          (index != dot) &&
          (m_data[0][index] >= x)) {
        // Handle edge case: 2 dots have same abscissa
        if (m_data[0][index] != x || (index > dot)) {
          nextX = m_data[0][index];
          selectedDot = index;
        }
      }
    }
    // Compare with the mean dot
    if (fabsf(meanOfColumn(0) - x) < fabsf(nextX - x) &&
          (m_numberOfPairs != dot) &&
          (meanOfColumn(0) >= x)) {
      if (meanOfColumn(0) != x || (x > dot)) {
        nextX = meanOfColumn(0);
        selectedDot = m_numberOfPairs;
      }
    }
  } else {
    // Compare with the mean dot
    if (fabsf(meanOfColumn(0) - x) < fabsf(nextX - x) &&
          (m_numberOfPairs != dot) &&
          (meanOfColumn(0) <= x)) {
      if (meanOfColumn(0) != x || (m_numberOfPairs < dot)) {
        nextX = meanOfColumn(0);
        selectedDot = m_numberOfPairs;
      }
    }
    for (int index = m_numberOfPairs-1; index >= 0; index--) {
      if (fabsf(m_data[0][index] - x) < fabsf(nextX - x) &&
          (index != dot) &&
          (m_data[0][index] <= x)) {
        // Handle edge case: 2 dots have same abscissa
        if (m_data[0][index] != x || (index < dot)) {
          nextX = m_data[0][index];
          selectedDot = index;
        }
      }
    }
  }
  return selectedDot;
}

/* Window */

void Store::setDefault() {
  float min = minValueOfColumn(0);
  float max = maxValueOfColumn(0);
  float range = max - min;
  m_xMin = min - k_displayLeftMarginRatio*range;
  m_xMax = max + k_displayRightMarginRatio*range;
  min = minValueOfColumn(1);
  max = maxValueOfColumn(1);
  range = max - min;
  m_yMin = min - k_displayBottomMarginRatio*range;
  m_yMax = max + k_displayTopMarginRatio*range;
  if (m_xMin == m_xMax) {;
    m_xMin = m_xMin - 1.0f;
    m_xMax = m_xMax + 1.0f;
  }
  if (m_yMin == m_yMax) {
    m_yMin = m_yMin - 1.0f;
    m_yMax = m_yMax + 1.0f;
  }
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

/* Calculations */

float Store::numberOfPairs() {
  return m_numberOfPairs;
}

float Store::maxValueOfColumn(int i) {
  float max = -FLT_MAX;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_data[i][k] > max) {
      max = m_data[i][k];
    }
  }
  return max;
}

float Store::minValueOfColumn(int i) {
  float min = FLT_MAX;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_data[i][k] < min) {
      min = m_data[i][k];
    }
  }
  return min;
}

float Store::squaredValueSumOfColumn(int i) {
  float result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_data[i][k]*m_data[i][k];
  }
  return result;
}

float Store::columnProductSum() {
  float result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_data[0][k]*m_data[1][k];
  }
  return result;
}

float Store::meanOfColumn(int i) {
  return sumOfColumn(i)/m_numberOfPairs;
}

float Store::varianceOfColumn(int i) {
  float mean = meanOfColumn(i);
  return squaredValueSumOfColumn(i)/m_numberOfPairs - mean*mean;
}

float Store::standardDeviationOfColumn(int i) {
  return sqrtf(varianceOfColumn(i));
}

float Store::covariance() {
  return columnProductSum()/m_numberOfPairs - meanOfColumn(0)*meanOfColumn(1);
}

float Store::slope() {
  return covariance()/varianceOfColumn(0);
}

float Store::yIntercept() {
  return meanOfColumn(1) - slope()*meanOfColumn(0);
}

float Store::yValueForXValue(float x) {
  return slope()*x+yIntercept();
}

float Store::xValueForYValue(float y) {
  return (y - yIntercept())/slope();
}

float Store::correlationCoefficient() {
  return covariance()/(standardDeviationOfColumn(0)*standardDeviationOfColumn(1));
}

float Store::squaredCorrelationCoefficient() {
  float cov = covariance();
  return cov*cov/(varianceOfColumn(0)*varianceOfColumn(1));
}

}
