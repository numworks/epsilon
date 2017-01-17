#include "interactive_curve_view_range.h"
#include <math.h>
#include <stddef.h>
#include <assert.h>
#include <ion.h>

InteractiveCurveViewRange::InteractiveCurveViewRange(CurveViewCursor * cursor, InteractiveCurveViewRangeDelegate * delegate) :
  MemoizedCurveViewRange(),
  m_yAuto(true),
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

bool InteractiveCurveViewRange::yAuto() {
  return m_yAuto;
}

void InteractiveCurveViewRange::setXMin(float xMin) {
  MemoizedCurveViewRange::setXMin(xMin);
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
}

void InteractiveCurveViewRange::setXMax(float xMax) {
  MemoizedCurveViewRange::setXMax(xMax);
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
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

void InteractiveCurveViewRange::panToMakePointVisible(float x, float y, float topMarginRatio, float rightMarginRatio, float bottomMarginRation, float leftMarginRation) {
  float xRange = m_xMax - m_xMin;
  float yRange = m_yMax - m_yMin;
  if (x < m_xMin + leftMarginRation*xRange) {
    m_xMin = x - leftMarginRation*xRange;
    m_xMax = m_xMin + xRange;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
    m_yAuto = false;
  }
  if (x > m_xMax - rightMarginRatio*xRange) {
    m_xMax = x + rightMarginRatio*xRange;
    m_xMin = m_xMax - xRange;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
    m_yAuto = false;
  }
  if (y < m_yMin + bottomMarginRation*yRange) {
    m_yMin = y - bottomMarginRation*yRange;
    m_yMax = m_yMin + yRange;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
    m_yAuto = false;
  }
  if (y > m_yMax - topMarginRatio*yRange) {
    m_yMax = y + topMarginRatio*yRange;
    m_yMin = m_yMax - yRange;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
    m_yAuto = false;
  }
}
