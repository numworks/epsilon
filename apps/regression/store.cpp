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

int Store::closestVerticalDot(int series, int direction, float x) {
  float nextX = INFINITY;
  float nextY = INFINITY;
  int selectedDot = -1;
  /* The conditions to test on all dots are in this order:
  * - the next dot should be within the window abscissa bounds
  * - the next dot is the closest one in abscissa to x
  * - the next dot is above the regression curve if direction == 1 and below
  * otherwise */
  for (int index = 0; index < m_numberOfPairs[series]; index++) {
    if ((m_xMin <= m_data[series][0][index] && m_data[series][0][index] <= m_xMax) &&
        (std::fabs(m_data[series][0][index] - x) < std::fabs(nextX - x)) &&
        ((m_data[series][1][index] - yValueForXValue(m_data[series][0][index]) >= 0) == (direction > 0))) {
      // Handle edge case: if 2 dots have the same abscissa but different ordinates
      if (nextX != m_data[series][0][index] || ((nextY - m_data[series][1][index] >= 0) == (direction > 0))) {
        nextX = m_data[series][0][index];
        nextY = m_data[series][1][index];
        selectedDot = index;
      }
    }
  }
  // Compare with the mean dot
  double meanX = meanOfColumn(series, 0);
  double meanY = meanOfColumn(series, 1);
  if (m_xMin <= meanX && meanX <= m_xMax &&
      (std::fabs(meanX - x) < std::fabs(nextX - x)) &&
      ((meanY - yValueForXValue(meanX) >= 0) == (direction > 0))) {
    if (nextX != meanX || ((nextY - meanY >= 0) == (direction > 0))) {
      selectedDot = m_numberOfPairs[series];
    }
  }
  return selectedDot;
}

int Store::nextDot(int direction, int dot) {
  float nextX = INFINITY;
  int selectedDot = -1;
  double meanX = meanOfColumn(series, 0);
  float x = meanX;
  if (dot >= 0 && dot < m_numberOfPairs[series]) {
    x = get(0, dot);
  }
  /* We have to scan the Store in opposite ways for the 2 directions to ensure to
   * select all dots (even with equal abscissa) */
  if (direction > 0) {
    for (int index = 0; index < m_numberOfPairs[series]; index++) {
      /* The conditions to test are in this order:
       * - the next dot is the closest one in abscissa to x
       * - the next dot is not the same as the selected one
       * - the next dot is at the right of the selected one */
      if (std::fabs(m_date[series][0][index] - x) < std::fabs(nextX - x) &&
          (index != dot) &&
          (m_date[series][0][index] >= x)) {
        // Handle edge case: 2 dots have same abscissa
        if (m_date[series][0][index] != x || (index > dot)) {
          nextX = m_date[series][0][index];
          selectedDot = index;
        }
      }
    }
    // Compare with the mean dot
    if (std::fabs(meanX - x) < std::fabs(nextX - x) &&
          (m_numberOfPairs[series] != dot) &&
          (meanX >= x)) {
      if (meanX != x || (x > dot)) {
        selectedDot = m_numberOfPairs[series];
      }
    }
  } else {
    // Compare with the mean dot
    if (std::fabs(meanX - x) < std::fabs(nextX - x) &&
          (m_numberOfPairs[series] != dot) &&
          (meanX <= x)) {
      if (meanX != x || (m_numberOfPairs[series] < dot)) {
        nextX = meanX;
        selectedDot = m_numberOfPairs[series];
      }
    }
    for (int index = m_numberOfPairs[series]-1; index >= 0; index--) {
      if (std::fabs(m_date[series][0][index] - x) < std::fabs(nextX - x) &&
          (index != dot) &&
          (m_date[series][0][index] <= x)) {
        // Handle edge case: 2 dots have same abscissa
        if (m_date[series][0][index] != x || (index < dot)) {
          nextX = m_date[series][0][index];
          selectedDot = index;
        }
      }
    }
  }
  return selectedDot;
}

/* Window */

void Store::setDefault(int series) {
  float min = minValueOfColumn(series, 0);
  float max = maxValueOfColumn(series, 0);
  float range = max - min;
  setXMin(min - k_displayLeftMarginRatio*range);
  setXMax(max + k_displayRightMarginRatio*range);
  setYAuto(true);
}

/* Calculations */

float Store::maxValueOfColumn(int series, int i) {
  float max = -FLT_MAX;
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    if (m_data[series][i][k] > max) {
      max = m_data[series][i][k];
    }
  }
  return max;
}

float Store::minValueOfColumn(int series, int i) {
  float min = FLT_MAX;
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    if (m_data[series][i][k] < min) {
      min = m_data[series][i][k];
    }
  }
  return min;
}

double Store::squaredValueSumOfColumn(int series, int i) {
  double result = 0;
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    result += m_data[series][i][k]*m_data[series][i][k];
  }
  return result;
}

double Store::columnProductSum(int series) {
  double result = 0;
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    result += m_date[series][0][k]*m_date[series][1][k];
  }
  return result;
}

double Store::meanOfColumn(int series, int i) {
  return m_numberOfPairs[series] == 0 ? 0 : sumOfColumn(series, i)/m_numberOfPairs[series];
}

double Store::varianceOfColumn(int series, int i) {
  double mean = meanOfColumn(series, i);
  return squaredValueSumOfColumn(series, i)/m_numberOfPairs[series] - mean*mean;
}

double Store::standardDeviationOfColumn(int series, int i) {
  return std::sqrt(varianceOfColumn(series, i));
}

double Store::covariance(int series) {
  return columnProductSum(series)/m_numberOfPairs[series] - meanOfColumn(series, 0)*meanOfColumn(series, 1);
}

double Store::slope(int series) {
  return covariance(series)/varianceOfColumn(series, 0);
}

double Store::yIntercept(int series) {
  return meanOfColumn(series, 1) - slope(series)*meanOfColumn(series, 0);
}

double Store::yValueForXValue(int series, double x) {
  return slope(series)*x+yIntercept(series);
}

double Store::xValueForYValue(int series, double y) {
  return std::fabs(slope(series)) < DBL_EPSILON ? NAN : (y - yIntercept(series))/slope(series);
}

double Store::correlationCoefficient(int series) {
  double sd0 = standardDeviationOfColumn(series, 0);
  double sd1 = standardDeviationOfColumn(series, 1);
  return (sd0 == 0.0 || sd1 == 0.0) ? 1.0 : covariance(series)/(sd0*sd1);
}

double Store::squaredCorrelationCoefficient(int series) {
  double cov = covariance(series);
  double v0 = varianceOfColumn(series, 0);
  double v1 = varianceOfColumn(series, 1);
  return (v0 == 0.0 || v1 == 0.0) ? 1.0 : cov*cov/(v0*v1);
}

InteractiveCurveViewRangeDelegate::Range Store::computeYRange(int series, InteractiveCurveViewRange * interactiveCurveViewRange) {
  float min = FLT_MAX;
  float max = -FLT_MAX;
  for (int k = 0; k < m_numberOfPairs[series]; k++) {
    if (m_xMin <= m_date[series][0][k] && m_date[series][0][k] <= m_xMax) {
      if (m_date[series][1][k] < min) {
        min = m_date[series][1][k];
      }
      if (m_date[series][1][k] > max) {
        max = m_date[series][1][k];
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
