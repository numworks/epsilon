#include "interactive_curve_view_range.h"
#include <poincare.h>
#include <ion.h>
#include <math.h>
#include <stddef.h>
#include <assert.h>

using namespace Poincare;

namespace Shared {

InteractiveCurveViewRange::InteractiveCurveViewRange(CurveViewCursor * cursor, InteractiveCurveViewRangeDelegate * delegate) :
  MemoizedCurveViewRange(),
  m_yAuto(true),
  m_delegate(delegate),
  m_cursor(cursor)
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
  float newXMin = xMin;
  MemoizedCurveViewRange::setXMin(clipped(newXMin, false));
  if (m_xMax - newXMin < k_minFloat) {
    newXMin = m_xMax - k_minFloat;
    MemoizedCurveViewRange::setXMin(clipped(newXMin, false));
  }
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
}

void InteractiveCurveViewRange::setXMax(float xMax) {
  float newXMax = xMax;
  MemoizedCurveViewRange::setXMax(clipped(newXMax, true));
  if (newXMax - m_xMin < k_minFloat) {
    newXMax = m_xMin + k_minFloat;
    MemoizedCurveViewRange::setXMax(clipped(newXMax, true));
  }
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
}

void InteractiveCurveViewRange::setYMin(float yMin) {
  float newYMin = yMin;
  MemoizedCurveViewRange::setYMin(clipped(newYMin, false));
  if (m_yMax - newYMin < k_minFloat) {
    newYMin = m_yMax - k_minFloat;
    MemoizedCurveViewRange::setYMin(clipped(newYMin, false));
  }
}

void InteractiveCurveViewRange::setYMax(float yMax) {
  float newYMax = yMax;
  MemoizedCurveViewRange::setYMax(clipped(newYMax, true));
  if (newYMax - m_yMin < k_minFloat) {
    newYMax = m_yMin + k_minFloat;
    MemoizedCurveViewRange::setYMax(clipped(newYMax, true));
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
  if (2.0f*ratio*fabsf(xMax-xMin) < k_minFloat || 2.0f*ratio*fabsf(yMax-yMin) < k_minFloat) {
    return;
  }
  float newXMin = clipped((xMax+xMin)/2.0f - ratio*fabsf(xMax-xMin), false);
  float newXMax = clipped((xMax+xMin)/2.0f + ratio*fabsf(xMax-xMin), true);
  if (!isnan(newXMin) && !isnan(newXMax)) {
    m_xMin = clipped((xMax+xMin)/2.0f - ratio*fabsf(xMax-xMin), false);
    m_xMax = clipped((xMax+xMin)/2.0f + ratio*fabsf(xMax-xMin), true);
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  }
  m_yAuto = false;
  float newYMin = clipped((yMax+yMin)/2.0f - ratio*fabsf(yMax-yMin), false);
  float newYMax = clipped((yMax+yMin)/2.0f + ratio*fabsf(yMax-yMin), true);
  if (!isnan(newYMin) && !isnan(newYMax)) {
    m_yMin = clipped((yMax+yMin)/2.0f - ratio*fabsf(yMax-yMin), false);
    m_yMax = clipped((yMax+yMin)/2.0f + ratio*fabsf(yMax-yMin), true);
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  }
}

void InteractiveCurveViewRange::panWithVector(float x, float y) {
  m_yAuto = false;
  if (clipped(m_xMin + x, false) != m_xMin + x || clipped(m_xMax + x, true) != m_xMax + x || clipped(m_yMin + y, false) != m_yMin + y || clipped(m_yMax + y, true) != m_yMax + y || isnan(clipped(m_xMin + x, false)) || isnan(clipped(m_xMax + x, true)) || isnan(clipped(m_yMin + y, false)) || isnan(clipped(m_yMax + y, true))) {
    return;
  }
  m_xMin = clipped(m_xMin + x, false);
  m_xMax = clipped(m_xMax + x, true);
  m_yMin = clipped(m_yMin + y, false);
  m_yMax = clipped(m_yMax + y, true);
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void InteractiveCurveViewRange::roundAbscissa() {
  float xMin = m_xMin;
  float xMax = m_xMax;
  float newXMin = clipped(roundf((xMin+xMax)/2) - (float)Ion::Display::Width/2.0f, false);
  float newXMax = clipped(roundf((xMin+xMax)/2) + (float)Ion::Display::Width/2.0f-1.0f, true);
  if (isnan(newXMin) || isnan(newXMax)) {
    return;
  }
  m_xMin = newXMin;
  m_xMax = newXMax;
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
  float newXMin = clipped((xMin+xMax)/2 - 5.3f, false);
  float newXMax = clipped((xMin+xMax)/2 + 5.3f, true);
  if (!isnan(newXMin) && !isnan(newXMax)) {
    m_xMin = newXMin;
    m_xMax = newXMax;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  }
  m_yAuto = false;
  float newYMin = clipped((yMin+yMax)/2 - 3.1f, false);
  float newYMax = clipped((yMin+yMax)/2 + 3.1f, true);
  if (!isnan(newYMin) && !isnan(newYMax)) {
    m_yMin = newYMin;
    m_yMax = newYMax;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  }
}

void InteractiveCurveViewRange::setTrigonometric() {
  m_xMin = -10.5f;
  m_xMax = 10.5f;
  if (Preferences::sharedPreferences()->angleUnit() == Expression::AngleUnit::Degree) {
    m_xMin = -600;
    m_xMax = 600;
  }
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
  if (isnan(position)) {
    return;
  }
  if (axis == Axis::X) {
    float range = m_xMax - m_xMin;
    m_xMin = clipped(position - range/2.0f, false);
    m_xMax = clipped(position + range/2.0f, true);
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  } else {
    m_yAuto = false;
    float range = m_yMax - m_yMin;
    m_yMin = clipped(position - range/2.0f, false);
    m_yMax = clipped(position + range/2.0f, true);
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  }
}

void InteractiveCurveViewRange::panToMakePointVisible(float x, float y, float topMarginRatio, float rightMarginRatio, float bottomMarginRation, float leftMarginRation) {
  float xRange = m_xMax - m_xMin;
  float yRange = m_yMax - m_yMin;
  if (x < m_xMin + leftMarginRation*xRange && !isinf(x) && !isnan(x)) {
    m_xMin = clipped(x - leftMarginRation*xRange, false);
    m_xMax = clipped(m_xMin + xRange, true);
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
    m_yAuto = false;
  }
  if (x > m_xMax - rightMarginRatio*xRange && !isinf(x) && !isnan(x)) {
    m_xMax = clipped(x + rightMarginRatio*xRange, true);
    m_xMin = clipped(m_xMax - xRange, false);
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
    m_yAuto = false;
  }
  if (y < m_yMin + bottomMarginRation*yRange && !isinf(y) && !isnan(y)) {
    m_yMin = clipped(y - bottomMarginRation*yRange, false);
    m_yMax = clipped(m_yMin + yRange, true);
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
    m_yAuto = false;
  }
  if (y > m_yMax - topMarginRatio*yRange && !isinf(y) && !isnan(y)) {
    m_yMax = clipped(y + topMarginRatio*yRange, true);
    m_yMin = clipped(m_yMax - yRange, false);
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
    m_yAuto = false;
  }
}

bool InteractiveCurveViewRange::isCursorVisible() {
  return m_cursor->x() >= m_xMin && m_cursor->x() <= m_xMax && m_cursor->y() >= m_yMin && m_cursor->y() <= m_yMax;
}

float InteractiveCurveViewRange::clipped(float x, bool isMax) {
  float max = isMax ? k_upperMaxFloat : k_lowerMaxFloat;
  float min = isMax ? -k_lowerMaxFloat : -k_upperMaxFloat;
  float clippedX = x > max ? max : x;
  clippedX = clippedX < min ? min : clippedX;
  return clippedX;
}

}
