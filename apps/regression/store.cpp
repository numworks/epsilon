#include "store.h"
#include <assert.h>
#include <float.h>
#include <cmath>
#include <string.h>

using namespace Shared;

namespace Regression {

Store::Store() :
  InteractiveCurveViewRange(nullptr, this),
  FloatPairStore()
{
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
        (std::fabs(m_data[0][index] - x) < std::fabs(nextX - x)) &&
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
      (std::fabs(meanOfColumn(0) - x) < std::fabs(nextX - x)) &&
      ((meanOfColumn(1) - yValueForXValue(meanOfColumn(0)) >= 0) == (direction > 0))) {
    if (nextX != meanOfColumn(0) || ((nextY - meanOfColumn(1) >= 0) == (direction > 0))) {
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
      if (std::fabs(m_data[0][index] - x) < std::fabs(nextX - x) &&
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
    if (std::fabs(meanOfColumn(0) - x) < std::fabs(nextX - x) &&
          (m_numberOfPairs != dot) &&
          (meanOfColumn(0) >= x)) {
      if (meanOfColumn(0) != x || (x > dot)) {
        selectedDot = m_numberOfPairs;
      }
    }
  } else {
    // Compare with the mean dot
    if (std::fabs(meanOfColumn(0) - x) < std::fabs(nextX - x) &&
          (m_numberOfPairs != dot) &&
          (meanOfColumn(0) <= x)) {
      if (meanOfColumn(0) != x || (m_numberOfPairs < dot)) {
        nextX = meanOfColumn(0);
        selectedDot = m_numberOfPairs;
      }
    }
    for (int index = m_numberOfPairs-1; index >= 0; index--) {
      if (std::fabs(m_data[0][index] - x) < std::fabs(nextX - x) &&
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
  setXMin(min - k_displayLeftMarginRatio*range);
  setXMax(max + k_displayRightMarginRatio*range);
  setYAuto(true);
}

/* Calculations */

double Store::numberOfPairs() {
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

double Store::squaredValueSumOfColumn(int i) {
  double result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_data[i][k]*m_data[i][k];
  }
  return result;
}

double Store::columnProductSum() {
  double result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_data[0][k]*m_data[1][k];
  }
  return result;
}

double Store::meanOfColumn(int i) {
  if (m_numberOfPairs == 0) {
    return 0;
  }
  return sumOfColumn(i)/m_numberOfPairs;
}

double Store::varianceOfColumn(int i) {
  double mean = meanOfColumn(i);
  return squaredValueSumOfColumn(i)/m_numberOfPairs - mean*mean;
}

double Store::standardDeviationOfColumn(int i) {
  return std::sqrt(varianceOfColumn(i));
}

double Store::covariance() {
  return columnProductSum()/m_numberOfPairs - meanOfColumn(0)*meanOfColumn(1);
}

double Store::slope() {
  return covariance()/varianceOfColumn(0);
}

double Store::yIntercept() {
  return meanOfColumn(1) - slope()*meanOfColumn(0);
}

double Store::yValueForXValue(double x) {
  return slope()*x+yIntercept();
}

double Store::xValueForYValue(double y) {
  if (std::fabs(slope()) < DBL_EPSILON) {
    return NAN;
  }
  return (y - yIntercept())/slope();
}

double Store::correlationCoefficient() {
  double sd0 = standardDeviationOfColumn(0);
  double sd1 = standardDeviationOfColumn(1);
  if (sd0 == 0.0 || sd1 == 0.0) {
    return 1.0;
  }
  return covariance()/(sd0*sd1);
}

double Store::squaredCorrelationCoefficient() {
  double cov = covariance();
  double v0 = varianceOfColumn(0);
  double v1 = varianceOfColumn(1);
  if (v0 == 0.0 || v1 == 0.0) {
    return 1.0;
  }
  return cov*cov/(v0*v1);
}

InteractiveCurveViewRangeDelegate::Range Store::computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) {
  float min = FLT_MAX;
  float max = -FLT_MAX;
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
  InteractiveCurveViewRangeDelegate::Range range;
  range.min = min;
  range.max = max;
  return range;
}

float Store::addMargin(float x, float range, bool isMin) {
  float ratio = isMin ? -k_displayBottomMarginRatio : k_displayTopMarginRatio;
  return x+ratio*range;
}

}
