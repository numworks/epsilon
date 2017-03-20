#include "interactive_curve_view_range.h"
#include <poincare.h>
#include <math.h>
#include <stddef.h>
#include <assert.h>
#include <ion.h>

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
  if (m_xMax - m_xMin < k_minFloat) {
    newXMin = m_xMax - k_minFloat;
  }
  MemoizedCurveViewRange::setXMin(CurveViewCursor::clipped(newXMin));
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
}

void InteractiveCurveViewRange::setXMax(float xMax) {
  float newXMax = xMax;
  if (m_xMax - m_xMin < k_minFloat) {
    newXMax = m_xMin + k_minFloat;
  }
  MemoizedCurveViewRange::setXMax(CurveViewCursor::clipped(newXMax));
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
}

void InteractiveCurveViewRange::setYMin(float yMin) {
  float newYMin = yMin;
  if (m_yMax - m_yMin < k_minFloat) {
    newYMin = m_yMax - k_minFloat;
  }
  MemoizedCurveViewRange::setYMin(CurveViewCursor::clipped(newYMin));
}

void InteractiveCurveViewRange::setYMax(float yMax) {
  float newYMax = yMax;
  if (m_yMax - m_yMin < k_minFloat) {
    newYMax = m_yMin + k_minFloat;
  }
  MemoizedCurveViewRange::setYMax(CurveViewCursor::clipped(newYMax));
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
  m_xMin = CurveViewCursor::clipped((xMax+xMin)/2.0f - ratio*fabsf(xMax-xMin));
  m_xMax = CurveViewCursor::clipped((xMax+xMin)/2.0f + ratio*fabsf(xMax-xMin));
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yAuto = false;
  m_yMin = CurveViewCursor::clipped((yMax+yMin)/2.0f - ratio*fabsf(yMax-yMin));
  m_yMax = CurveViewCursor::clipped((yMax+yMin)/2.0f + ratio*fabsf(yMax-yMin));
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void InteractiveCurveViewRange::panWithVector(float x, float y) {
  m_yAuto = false;
  if (CurveViewCursor::clipped(m_xMin + x) != m_xMin + x || CurveViewCursor::clipped(m_xMax + x) != m_xMax + x || CurveViewCursor::clipped(m_yMin + y) != m_yMin + y || CurveViewCursor::clipped(m_yMax + y) != m_yMax + y) {
    return;
  }
  m_xMin = CurveViewCursor::clipped(m_xMin + x);
  m_xMax = CurveViewCursor::clipped(m_xMax + x);
  m_yMin = CurveViewCursor::clipped(m_yMin + y);
  m_yMax = CurveViewCursor::clipped(m_yMax + y);
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
  if (axis == Axis::X) {
    float range = m_xMax - m_xMin;
    m_xMin = CurveViewCursor::clipped(position - range/2.0f);
    m_xMax = CurveViewCursor::clipped(position + range/2.0f);
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  } else {
    m_yAuto = false;
    float range = m_yMax - m_yMin;
    m_yMin = CurveViewCursor::clipped(position - range/2.0f);
    m_yMax = CurveViewCursor::clipped(position + range/2.0f);
    m_yGridUnit = CurveViewCursor::clipped(computeGridUnit(Axis::Y, m_yMin, m_yMax));
  }
}

void InteractiveCurveViewRange::panToMakePointVisible(float x, float y, float topMarginRatio, float rightMarginRatio, float bottomMarginRation, float leftMarginRation) {
  float xRange = m_xMax - m_xMin;
  float yRange = m_yMax - m_yMin;
  if (x < m_xMin + leftMarginRation*xRange && !isinf(x) && !isnan(x)) {
    m_xMin = CurveViewCursor::clipped(x - leftMarginRation*xRange);
    m_xMax = m_xMin + xRange;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
    m_yAuto = false;
  }
  if (x > m_xMax - rightMarginRatio*xRange && !isinf(x) && !isnan(x)) {
    m_xMax = CurveViewCursor::clipped(x + rightMarginRatio*xRange);
    m_xMin = m_xMax - xRange;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
    m_yAuto = false;
  }
  if (y < m_yMin + bottomMarginRation*yRange && !isinf(y) && !isnan(y)) {
    m_yMin = CurveViewCursor::clipped(y - bottomMarginRation*yRange);
    m_yMax = m_yMin + yRange;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
    m_yAuto = false;
  }
  if (y > m_yMax - topMarginRatio*yRange && !isinf(y) && !isnan(y)) {
    m_yMax = CurveViewCursor::clipped(y + topMarginRatio*yRange);
    m_yMin = m_yMax - yRange;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
    m_yAuto = false;
  }
}

}
