#include "data.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <string.h>

namespace Regression {

Data::Data() :
  ::Data(),
  m_xCursorPosition(NAN),
  m_yCursorPosition(NAN),
  m_dotsSelected(false),
  m_xMin(0.0f),
  m_xMax(10.0f),
  m_yMin(0.0f),
  m_yMax(10.0f),
  m_xGridUnit(1.0f),
  m_yGridUnit(1.0f)
{
}

/* Raw numeric data */

float Data::xValueAtIndex(int index) {
  assert(index < m_numberOfPairs);
  return m_xValues[index];
}

float Data::yValueAtIndex(int index) {
  assert(index < m_numberOfPairs);
  return m_yValues[index];
}

void Data::setXValueAtIndex(float value, int index) {
  if (index >= k_maxNumberOfPairs) {
    return;
  }
  m_xValues[index] = value;
  if (index >= m_numberOfPairs) {
    m_yValues[index] = 0.0f;
    m_numberOfPairs++;
  }
  initCursorPosition();
  initWindowParameters();
}

void Data::setYValueAtIndex(float value, int index) {
  if (index >= k_maxNumberOfPairs) {
    return;
  }
  m_yValues[index] = value;
  if (index >= m_numberOfPairs) {
    m_xValues[index] = 0.0f;
    m_numberOfPairs++;
  }
  initCursorPosition();
  initWindowParameters();
}

void Data::deletePairAtIndex(int index) {
  m_numberOfPairs--;
  for (int k = index; k < m_numberOfPairs; k++) {
    m_xValues[k] = m_xValues[k+1];
    m_yValues[k] = m_yValues[k+1];
  }
  m_xValues[m_numberOfPairs] = 0.0f;
  m_yValues[m_numberOfPairs] = 0.0f;
  initCursorPosition();
  initWindowParameters();
}

/* Cursor */

float Data::xCursorPosition() {
  return m_xCursorPosition;
}

float Data::yCursorPosition() {
  return m_yCursorPosition;
}

bool Data::cursorSelectUp() {
  float yRegressionCurve = yValueForXValue(m_xCursorPosition);
  if (m_dotsSelected && m_yCursorPosition < yRegressionCurve) {
    m_dotsSelected = false;
    m_yCursorPosition = yRegressionCurve;
    return true;
  }
  if (!m_dotsSelected) {
    if (selectClosestDotRelativelyToCurve(1)) {
      m_dotsSelected = true;
      return true;
    }
  }
  return false;
}

bool Data::cursorSelectBottom() {
  float yRegressionCurve = yValueForXValue(m_xCursorPosition);
  if (m_dotsSelected && m_yCursorPosition > yRegressionCurve) {
    m_dotsSelected = false;
    m_yCursorPosition = yRegressionCurve;
    return true;
  }
  if (!m_dotsSelected) {
    if (selectClosestDotRelativelyToCurve(-1)) {
      m_dotsSelected = true;
      return true;
    }
  }
  return false;
}

bool Data::cursorSelectLeft() {
  if (m_dotsSelected) {
    return selectNextDot(-1);
  } else {
    m_xCursorPosition -= m_xGridUnit;
    m_yCursorPosition = yValueForXValue(m_xCursorPosition);
    return true;
  }
}

bool Data::cursorSelectRight() {
  if (m_dotsSelected) {
    return selectNextDot(1);
  } else {
    m_xCursorPosition += m_xGridUnit;
    m_yCursorPosition = yValueForXValue(m_xCursorPosition);
    return true;
  }
}

/* CurveViewWindow */

float Data::xMin() {
  return m_xMin;
}

float Data::xMax() {
  return m_xMax;
}

float Data::yMin() {
  return m_yMin;
}

float Data::yMax() {
  return m_yMax;
}

float Data::xGridUnit() {
  return m_xGridUnit;
}

float Data::yGridUnit() {
  return m_yGridUnit;
}

/* Calculations */

float Data::xSum() {
  float result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_xValues[k];
  }
  return result;
}

float Data::ySum() {
  float result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_yValues[k];
  }
  return result;
}

float Data::xSquaredValueSum() {
  float result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_xValues[k]*m_xValues[k];
  }
  return result;
}

float Data::ySquaredValueSum() {
  float result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_yValues[k]*m_yValues[k];
  }
  return result;
}

float Data::xyProductSum() {
  float result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    result += m_xValues[k]*m_yValues[k];
  }
  return result;
}

float Data::xMean() {
  return xSum()/m_numberOfPairs;
}

float Data::yMean() {
  return ySum()/m_numberOfPairs;
}

float Data::xVariance() {
  float mean = xMean();
  return xSquaredValueSum()/m_numberOfPairs - mean*mean;
}

float Data::yVariance() {
  float mean = yMean();
  return ySquaredValueSum()/m_numberOfPairs - mean*mean;
}

float Data::xStandardDeviation() {
  return sqrtf(xVariance());
}

float Data::yStandardDeviation() {
  return sqrtf(yVariance());
}

float Data::covariance() {
  return xyProductSum()/m_numberOfPairs - xMean()*yMean();
}

float Data::slope() {
  return covariance()/xVariance();
}

float Data::yIntercept() {
  return yMean() - slope()*xMean();
}

float Data::yValueForXValue(float x) {
  return slope()*x+yIntercept();
}

float Data::correlationCoefficient() {
  return covariance()/(xStandardDeviation()*yStandardDeviation());
}

float Data::squaredCorrelationCoefficient() {
  float cov = covariance();
  return cov*cov/(xVariance()*yVariance());
}

float Data::maxXValue() {
  float max = -FLT_MAX;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_xValues[k] > max) {
      max = m_xValues[k];
    }
  }
  return max;
}

float Data::maxYValue() {
  float max = -FLT_MAX;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_yValues[k] > max) {
      max = m_yValues[k];
    }
  }
  return max;
}

float Data::minXValue() {
  float min = FLT_MAX;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_xValues[k] < min) {
      min = m_xValues[k];
    }
  }
  return min;
}

float Data::minYValue() {
  float min = FLT_MAX;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_yValues[k] < min) {
      min = m_yValues[k];
    }
  }
  return min;
}

void Data::initCursorPosition() {
  m_xCursorPosition = (m_xMin+m_xMax)/2.0f;
  m_yCursorPosition = yValueForXValue(m_xCursorPosition);
  m_dotsSelected = false;
}

void Data::initWindowParameters() {
  m_xMin = minXValue();
  m_xMax = maxXValue();
  m_yMin = minYValue();
  m_yMax = maxYValue();
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

bool Data::selectClosestDotRelativelyToCurve(int direction) {
  float nextX = INFINITY;
  float nextY = INFINITY;
  for (int index = 0; index < m_numberOfPairs; index++) {
    if (fabsf(m_xValues[index] - m_xCursorPosition) < fabsf(nextX - m_xCursorPosition) &&
        ((direction > 0 && m_yValues[index] >= yValueForXValue(m_xValues[index])) ||  (direction < 0 && m_yValues[index] <= yValueForXValue(m_xValues[index])))) {
      nextX = m_xValues[index];
      nextY = m_yValues[index];
    }
  }
  if (!isinf(nextX) && !isinf(nextY)) {
    m_xCursorPosition = nextX;
    m_yCursorPosition = nextY;
    return true;
  }
  return false;
}

bool Data::selectNextDot(int direction) {
  float nextX = INFINITY;
  float nextY = INFINITY;
  for (int index = 0; index < m_numberOfPairs; index++) {
    if (fabsf(m_xValues[index] - m_xCursorPosition) < fabsf(nextX - m_xCursorPosition) &&
        ((direction > 0 && m_xValues[index] > m_xCursorPosition) ||  (direction < 0 && m_xValues[index] < m_xCursorPosition))) {
      nextX = m_xValues[index];
      nextY = m_yValues[index];
    }
  }
  if (!isinf(nextX) && !isinf(nextY)) {
    m_xCursorPosition = nextX;
    m_yCursorPosition = nextY;
    return true;
  }
  return false;
}

}
