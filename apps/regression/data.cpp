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
}

void Data::initWindowParameters() {
  m_xMin = minXValue();
  m_xMax = maxXValue();
  m_yMin = minYValue();
  m_yMax = maxYValue();
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

}
