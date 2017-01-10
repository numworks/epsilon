#include "interactive_curve_view_range.h"
#include <math.h>
#include <stddef.h>
#include <assert.h>
#include <ion.h>

InteractiveCurveViewRange::InteractiveCurveViewRange(CurveViewCursor * cursor, InteractiveCurveViewRangeDelegate * delegate) :
  m_xMin(-10.0f),
  m_xMax(10.0f),
  m_yMin(-10.0f),
  m_yMax(10.0f),
  m_yAuto(true),
  m_xGridUnit(2.0f),
  m_yGridUnit(2.0f),
  m_cursor(cursor),
  m_delegate(delegate)
{
}

void InteractiveCurveViewRange::setCursor(CurveViewCursor * cursor) {
  m_cursor = cursor;
}

uint32_t InteractiveCurveViewRange::rangeChecksum() {
  float data[5] = {m_xMin, m_xMax, m_yMin, m_yMax, m_yAuto ? 1.0f : 0.0f};
  size_t dataLengthInBytes = 5*sizeof(float);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  return Ion::crc32((uint32_t *)data, dataLengthInBytes>>2);
}

float InteractiveCurveViewRange::xMin() {
  return m_xMin;
}

float InteractiveCurveViewRange::xMax() {
  return m_xMax;
}

float InteractiveCurveViewRange::yMin() {
  return m_yMin;
}

float InteractiveCurveViewRange::yMax() {
  return m_yMax;
}

bool InteractiveCurveViewRange::yAuto() {
  return m_yAuto;
}

float InteractiveCurveViewRange::xGridUnit() {
  return m_xGridUnit;
}

float InteractiveCurveViewRange::yGridUnit() {
  return m_yGridUnit;
}

void InteractiveCurveViewRange::setXMin(float xMin) {
  m_xMin = xMin;
  if (m_xMin >= m_xMax) {
    m_xMax = xMin + 1.0f;
  }
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
}

void InteractiveCurveViewRange::setXMax(float xMax) {
  m_xMax = xMax;
  if (m_xMin >= m_xMax) {
    m_xMin = xMax - 1.0f;
  }
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
}

void InteractiveCurveViewRange::setYMin(float yMin) {
  m_yMin = yMin;
  if (m_yMin >= m_yMax) {
    m_yMax = yMin + 1.0f;
  }
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void InteractiveCurveViewRange::setYMax(float yMax) {
  m_yMax = yMax;
  if (m_yMin >= m_yMax) {
    m_yMin = yMax - 1.0f;
  }
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void InteractiveCurveViewRange::setYAuto(bool yAuto) {
  m_yAuto = yAuto;
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
}

void InteractiveCurveViewRange::zoom(float ratio) {
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
}

void InteractiveCurveViewRange::panWithVector(float x, float y) {
  m_yAuto = false;
  m_xMin = m_xMin + x;
  m_xMax = m_xMax + x;
  m_yMin = m_yMin + y;
  m_yMax = m_yMax + y;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void InteractiveCurveViewRange::roundAbscissa() {
  float xMin = m_xMin;
  float xMax = m_xMax;
  m_xMin = roundf((xMin+xMax)/2) - (float)Ion::Display::Width/2.0f;
  m_xMax = roundf((xMin+xMax)/2) + (float)Ion::Display::Width/2.0f-1.0f;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
}

void InteractiveCurveViewRange::normalize() {
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
}

void InteractiveCurveViewRange::setTrigonometric() {
  // TODO: if mode == degree, xmin = -600, xmax = 600
  m_xMin = -10.5f;
  m_xMax = 10.5f;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yAuto = false;
  m_yMin = -1.6f;
  m_yMax = 1.6f;
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void InteractiveCurveViewRange::setDefault() {
  m_xMin = -10.0f;
  m_xMax = 10.0f;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  setYAuto(true);
}

void InteractiveCurveViewRange::centerAxisAround(Axis axis, float position) {
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

bool InteractiveCurveViewRange::moveCursorTo(float x, float y) {
  if (m_cursor) {
    m_cursor->moveTo(x, y);
    float xMargin = InteractiveCurveViewRange::k_cursorMarginFactorToBorder * (m_xMax - m_xMin);
    float yMargin = InteractiveCurveViewRange::k_cursorMarginFactorToBorder * (m_yMax - m_yMin);
    bool windowHasMoved = panToMakePointVisible(x, y, xMargin, yMargin);
    return windowHasMoved;
  }
  return false;
}

bool InteractiveCurveViewRange::panToMakePointVisible(float x, float y, float xMargin, float yMargin) {
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
