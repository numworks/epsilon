#include "store.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <string.h>

namespace Regression {

Store::Store() :
  CurveViewWindowWithCursor(),
  FloatPairStore(),
  m_selectedDotIndex(-1)
{
}

/* Cursor */

void Store::initCursorPosition() {
  m_xCursorPosition = (m_xMin+m_xMax)/2.0f;
  m_yCursorPosition = yValueForXValue(m_xCursorPosition);
  m_selectedDotIndex = -1;
}

int Store::moveCursorVertically(int direction) {
  float yRegressionCurve = yValueForXValue(m_xCursorPosition);
  if (m_selectedDotIndex >= 0) {
    if ((yRegressionCurve - m_yCursorPosition > 0) == (direction > 0)) {
      m_selectedDotIndex = -1;
      m_yCursorPosition = yRegressionCurve;
    } else {
      return -1;
    }
  } else {
    int dotSelected = selectClosestDotRelativelyToCurve(direction);
    if (dotSelected >= 0) {
      m_selectedDotIndex = dotSelected;
    } else {
      return -1;
    }
  }
  bool windowHasMoved = panToMakePointVisible(m_xCursorPosition, m_yCursorPosition, 0.0f, 0.0f);
  return windowHasMoved;
}

int Store::moveCursorHorizontally(int direction) {
  if (m_selectedDotIndex >= 0) {
    int dotSelected = selectNextDot(direction);
    if (dotSelected >= 0) {
      m_selectedDotIndex = dotSelected;
    } else {
      return -1;
    }
  } else {
    m_xCursorPosition = direction > 0 ? m_xCursorPosition + m_xGridUnit/CurveViewWindowWithCursor::k_numberOfCursorStepsInGradUnit :
      m_xCursorPosition - m_xGridUnit/CurveViewWindowWithCursor::k_numberOfCursorStepsInGradUnit;
    m_yCursorPosition = yValueForXValue(m_xCursorPosition);
  }
  bool windowHasMoved = panToMakePointVisible(m_xCursorPosition, m_yCursorPosition, 0.0f, 0.0f);
  return windowHasMoved;
}

int Store::selectedDotIndex() {
  return m_selectedDotIndex;
}

void Store::setCursorPositionAtAbscissa(float abscissa) {
  m_xCursorPosition = abscissa;
  centerAxisAround(CurveViewWindow::Axis::X, m_xCursorPosition);
  m_yCursorPosition = yValueForXValue(m_xCursorPosition);
  centerAxisAround(CurveViewWindow::Axis::Y, m_yCursorPosition);
}

void Store::setCursorPositionAtOrdinate(float ordinate) {
  m_yCursorPosition = ordinate;
  centerAxisAround(CurveViewWindow::Axis::Y, m_yCursorPosition);
  m_xCursorPosition = xValueForYValue(m_yCursorPosition);
  centerAxisAround(CurveViewWindow::Axis::X, m_xCursorPosition);
}

/* Window */

void Store::initWindowParameters() {
  m_xMin = minValueOfColumn(0);
  m_xMax = maxValueOfColumn(0);
  m_yMin = minValueOfColumn(1);
  m_yMax = maxValueOfColumn(1);
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

void Store::setDefault() {
  initWindowParameters();
}

/* Calculations */

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

/* private methods */

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

bool Store::computeYaxis() {
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

int Store::selectClosestDotRelativelyToCurve(int direction) {
  float nextX = INFINITY;
  float nextY = INFINITY;
  int selectedDot = -1;
  /* The conditions to test on all dots are in this order:
  * - the next dot should be within the window abscissa bounds
  * - the next dot is the closest one in abscissa
  * - the next dot is above the selected one if direction == 1 and below
  * otherwise */
  for (int index = 0; index < m_numberOfPairs; index++) {
    if ((m_xMin <= m_data[0][index] && m_data[0][index] <= m_xMax) &&
        (fabsf(m_data[0][index] - m_xCursorPosition) < fabsf(nextX - m_xCursorPosition)) &&
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
      (fabsf(meanOfColumn(0) - m_xCursorPosition) < fabsf(nextX - m_xCursorPosition)) &&
      ((meanOfColumn(1) - yValueForXValue(meanOfColumn(0)) >= 0) == (direction > 0))) { 
    if (nextX != meanOfColumn(0) || ((nextY - meanOfColumn(1) >= 0) == (direction > 0))) {
      nextX = meanOfColumn(0);
      nextY = meanOfColumn(1);
      selectedDot = m_numberOfPairs;
    }
  }
  if (!isinf(nextX) && !isinf(nextY)) {
    m_xCursorPosition = nextX;
    m_yCursorPosition = nextY;
    return selectedDot;
  }
  return selectedDot;
}

int Store::selectNextDot(int direction) {
  float nextX = INFINITY;
  float nextY = INFINITY;
  int selectedDot = -1;
  /* We have to scan the Store in opposite ways for the 2 directions to ensure to
   * select all dots (even with equal abscissa) */
  if (direction > 0) {
    for (int index = 0; index < m_numberOfPairs; index++) {
      /* The conditions to test are in this order:
       * - the next dot is the closest one in abscissa
       * - the next dot is not the same as the selected one
       * - the next dot is at the right of the selected one */
      if (fabsf(m_data[0][index] - m_xCursorPosition) < fabsf(nextX - m_xCursorPosition) &&
          (index != m_selectedDotIndex) &&
          (m_data[0][index] >= m_xCursorPosition)) {
        // Handle edge case: 2 dots have same abscissa
        if (m_data[0][index] != m_xCursorPosition || (index > m_selectedDotIndex)) {
          nextX = m_data[0][index];
          nextY = m_data[1][index];
          selectedDot = index;
        }
      }
    }
    // Compare with the mean dot
    if (fabsf(meanOfColumn(0) - m_xCursorPosition) < fabsf(nextX - m_xCursorPosition) &&
          (m_numberOfPairs != m_selectedDotIndex) &&
          (meanOfColumn(0) >= m_xCursorPosition)) {
      if (meanOfColumn(0) != m_xCursorPosition || (m_numberOfPairs > m_selectedDotIndex)) {
        nextX = meanOfColumn(0);
        nextY = meanOfColumn(1);
        selectedDot = m_numberOfPairs;
      }
    }
  } else {
    // Compare with the mean dot
    if (fabsf(meanOfColumn(0) - m_xCursorPosition) < fabsf(nextX - m_xCursorPosition) &&
          (m_numberOfPairs != m_selectedDotIndex) &&
          (meanOfColumn(0) <= m_xCursorPosition)) {
      if (meanOfColumn(0) != m_xCursorPosition || (m_numberOfPairs < m_selectedDotIndex)) {
        nextX = meanOfColumn(0);
        nextY = meanOfColumn(1);
        selectedDot = m_numberOfPairs;
      }
    }
    for (int index = m_numberOfPairs-1; index >= 0; index--) {
      if (fabsf(m_data[0][index] - m_xCursorPosition) < fabsf(nextX - m_xCursorPosition) &&
          (index != m_selectedDotIndex) &&
          (m_data[0][index] <= m_xCursorPosition)) {
        // Handle edge case: 2 dots have same abscissa
        if (m_data[0][index] != m_xCursorPosition || (index < m_selectedDotIndex)) {
          nextX = m_data[0][index];
          nextY = m_data[1][index];
          selectedDot = index;
        }
      }
    }
  }
  if (!isinf(nextX) && !isinf(nextY)) {
    m_xCursorPosition = nextX;
    m_yCursorPosition = nextY;
    return selectedDot;
  }
  return selectedDot;
}

}
