#include "curve_view_window_with_cursor.h"
#include <math.h>

CurveViewWindowWithCursor::CurveViewWindowWithCursor() :
  m_xCursorPosition(NAN),
  m_yCursorPosition(NAN),
  m_xMin(-10.0f),
  m_xMax(10.0f),
  m_yMin(-10.0f),
  m_yMax(10.0f),
  m_yAuto(true),
  m_xGridUnit(2.0f),
  m_yGridUnit(2.0f)
{
}

float CurveViewWindowWithCursor::xCursorPosition() {
  return m_xCursorPosition;
}

float CurveViewWindowWithCursor::yCursorPosition() {
  return m_yCursorPosition;
}

float CurveViewWindowWithCursor::xMin() {
  return m_xMin;
}

float CurveViewWindowWithCursor::xMax() {
  return m_xMax;
}

float CurveViewWindowWithCursor::yMin() {
  return m_yMin;
}

float CurveViewWindowWithCursor::yMax() {
  return m_yMax;
}

bool CurveViewWindowWithCursor::yAuto() {
  return m_yAuto;
}

float CurveViewWindowWithCursor::xGridUnit() {
  return m_xGridUnit;
}

float CurveViewWindowWithCursor::yGridUnit() {
  return m_yGridUnit;
}

void CurveViewWindowWithCursor::setXMin(float xMin) {
  m_xMin = xMin;
  computeYaxis();
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  initCursorPosition();
}

void CurveViewWindowWithCursor::setXMax(float xMax) {
  m_xMax = xMax;
  computeYaxis();
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  initCursorPosition();
}

void CurveViewWindowWithCursor::setYMin(float yMin) {
  m_yMin = yMin;
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  initCursorPosition();
}

void CurveViewWindowWithCursor::setYMax(float yMax) {
  m_yMax = yMax;
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  initCursorPosition();
}

void CurveViewWindowWithCursor::setYAuto(bool yAuto) {
  m_yAuto = yAuto;
  computeYaxis();
}

void CurveViewWindowWithCursor::zoom(float ratio) {
  float xMin = m_xMin;
  float xMax = m_xMax;
  float yMin = m_yMin;
  float yMax = m_yMax;
  m_xMin = (xMax+xMin)/2.0f - ratio*fabsf(xMax-xMin);
  m_xMax = (xMax+xMin)/2.0f + ratio*fabsf(xMax-xMin);
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yAuto = false;
  m_yMin = (yMax+yMin)/2.0f - ratio*fabsf(yMax-yMin);
  m_yMax = (yMax+yMin)/2.0f + ratio*fabsf(yMax-yMin);
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  initCursorPosition();
}

void CurveViewWindowWithCursor::translateWindow(Direction direction) {
  m_yAuto = false;
  if (direction == Direction::Up) {
    m_yMin = m_yMin + m_yGridUnit;
    m_yMax = m_yMax + m_yGridUnit;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  }
  if (direction == Direction::Down) {
    m_yMin = m_yMin - m_yGridUnit;
    m_yMax = m_yMax - m_yGridUnit;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  }
  if (direction == Direction::Left) {
    m_xMin = m_xMin - m_xGridUnit;
    m_xMax = m_xMax - m_xGridUnit;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  }
  if (direction == Direction::Right) {
    m_xMin = m_xMin + m_xGridUnit;
    m_xMax = m_xMax + m_xGridUnit;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  }
  initCursorPosition();
}

void CurveViewWindowWithCursor::roundAbscissa() {
  float xMin = m_xMin;
  float xMax = m_xMax;
  m_xMin = roundf((xMin+xMax)/2) - 160.0f;
  m_xMax = roundf((xMin+xMax)/2) + 159.0f;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  computeYaxis();
  initCursorPosition();
}

void CurveViewWindowWithCursor::normalize() {
  float xMin = m_xMin;
  float xMax = m_xMax;
  float yMin = m_yMin;
  float yMax = m_yMax;
  m_xMin = (xMin+xMax)/2 - 5.3f;
  m_xMax = (xMin+xMax)/2 + 5.3f;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yAuto = false;
  m_yMin = (yMin+yMax)/2 - 3.1f;
  m_yMax = (yMin+yMax)/2 + 3.1f;
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  initCursorPosition();
}

bool CurveViewWindowWithCursor::panToMakePointVisible(float x, float y, float xMargin, float yMargin) {
  bool windowMoved = false;
  float xRange = m_xMax - m_xMin;
  float yRange = m_yMax - m_yMin;
  if (x < m_xMin + xMargin) {
    m_xMin = x - xMargin;
    m_xMax = m_xMin + xRange;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
    m_yAuto = false;
    windowMoved = true;
  }
  if (x > m_xMax - xMargin) {
    m_xMax = x + xMargin;
    m_xMin = m_xMax - xRange;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
    m_yAuto = false;
    windowMoved = true;
  }
  if (y < m_yMin + yMargin) {
    m_yMin = y - yMargin;
    m_yMax = m_yMin + yRange;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
    m_yAuto = false;
    windowMoved = true;
  }
  if (y > m_yMax - yMargin) {
    m_yMax = y + yMargin;
    m_yMin = m_yMax - yRange;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
    m_yAuto = false;
    windowMoved = true;
  }
  return windowMoved;
}

void CurveViewWindowWithCursor::centerAxisAround(Axis axis, float position) {
  if (axis == Axis::X) {
    float range = m_xMax - m_xMin;
    m_xMin = position - range/2.0f;
    m_xMax = position + range/2.0f;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  } else {
    m_yAuto = false;
    float range = m_yMax - m_yMin;
    m_yMin = position - range/2.0f;
    m_yMax = position + range/2.0f;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  }
}
