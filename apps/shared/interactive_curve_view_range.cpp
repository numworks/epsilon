#include "interactive_curve_view_range.h"
#include <ion.h>
#include <cmath>
#include <stddef.h>
#include <assert.h>
#include <poincare/preferences.h>
#include <algorithm>

using namespace Poincare;

namespace Shared {

uint32_t InteractiveCurveViewRange::rangeChecksum() {
  float data[5] = {xMin(), xMax(), yMin(), yMax(), m_yAuto ? 1.0f : 0.0f};
  size_t dataLengthInBytes = 5*sizeof(float);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  return Ion::crc32Word((uint32_t *)data, dataLengthInBytes/sizeof(uint32_t));
}

void InteractiveCurveViewRange::setYAuto(bool yAuto) {
  m_yAuto = yAuto;
  notifyRangeChange();
}

void InteractiveCurveViewRange::setXMin(float xMin) {
  MemoizedCurveViewRange::protectedSetXMin(xMin, k_lowerMaxFloat, k_upperMaxFloat);
  notifyRangeChange();
}

void InteractiveCurveViewRange::setXMax(float xMax) {
  MemoizedCurveViewRange::protectedSetXMax(xMax, k_lowerMaxFloat, k_upperMaxFloat);
  notifyRangeChange();
}

void InteractiveCurveViewRange::setYMin(float yMin) {
  MemoizedCurveViewRange::protectedSetYMin(yMin, k_lowerMaxFloat, k_upperMaxFloat);
}

void InteractiveCurveViewRange::setYMax(float yMax) {
  MemoizedCurveViewRange::protectedSetYMax(yMax, k_lowerMaxFloat, k_upperMaxFloat);
}

void InteractiveCurveViewRange::zoom(float ratio, float x, float y) {
  float xMi = xMin();
  float xMa = xMax();
  float yMi = yMin();
  float yMa = yMax();
  if (ratio*std::fabs(xMa-xMi) < Range1D::k_minFloat || ratio*std::fabs(yMa-yMi) < Range1D::k_minFloat) {
    return;
  }
  float centerX = std::isnan(x) || std::isinf(x) ? xCenter() : x;
  float centerY = std::isnan(y) || std::isinf(y) ? yCenter() : y;
  float newXMin = centerX*(1.0f-ratio)+ratio*xMi;
  float newXMax = centerX*(1.0f-ratio)+ratio*xMa;
  m_yAuto = false;
  if (!std::isnan(newXMin) && !std::isnan(newXMax)) {
    m_xRange.setMax(newXMax, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetXMin(newXMin, k_lowerMaxFloat, k_upperMaxFloat);
  }
  float newYMin = centerY*(1.0f-ratio)+ratio*yMi;
  float newYMax = centerY*(1.0f-ratio)+ratio*yMa;
  if (!std::isnan(newYMin) && !std::isnan(newYMax)) {
    m_yRange.setMax(newYMax, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetYMin(newYMin, k_lowerMaxFloat, k_upperMaxFloat);
  }
}

void InteractiveCurveViewRange::panWithVector(float x, float y) {
  m_yAuto = false;
  if (clipped(xMin() + x, false) != xMin() + x || clipped(xMax() + x, true) != xMax() + x || clipped(yMin() + y, false) != yMin() + y || clipped(yMax() + y, true) != yMax() + y || std::isnan(clipped(xMin() + x, false)) || std::isnan(clipped(xMax() + x, true)) || std::isnan(clipped(yMin() + y, false)) || std::isnan(clipped(yMax() + y, true))) {
    return;
  }
  m_xRange.setMax(xMax()+x, k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetXMin(xMin() + x, k_lowerMaxFloat, k_upperMaxFloat);
  m_yRange.setMax(yMax()+y, k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetYMin(yMin() + y, k_lowerMaxFloat, k_upperMaxFloat);
}

void InteractiveCurveViewRange::roundAbscissa() {
  // Set x range
  float newXMin = std::round(xCenter()) - (float)Ion::Display::Width/2.0f;
  float newXMax = std::round(xCenter()) + (float)Ion::Display::Width/2.0f-1.0f;
  if (std::isnan(newXMin) || std::isnan(newXMax)) {
    return;
  }
  m_xRange.setMax(newXMax, k_lowerMaxFloat, k_upperMaxFloat);
  setXMin(newXMin);
}

void InteractiveCurveViewRange::normalize() {
  /* We center the ranges on the current range center, and put each axis so that
   * 1cm = 2 current units. */
  m_yAuto = false;

  const float unit = std::max(xGridUnit(), yGridUnit());

  // Set x range
  float newXHalfRange = NormalizedXHalfRange(unit);
  float newXMin = xCenter() - newXHalfRange;
  float newXMax = xCenter() + newXHalfRange;
  if (!std::isnan(newXMin) && !std::isnan(newXMax)) {
    m_xRange.setMax(newXMax, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetXMin(newXMin, k_lowerMaxFloat, k_upperMaxFloat);
  }
  // Set y range
  float newYHalfRange = NormalizedYHalfRange(unit);
  float newYMin = yCenter() - newYHalfRange;
  float newYMax = yCenter() + newYHalfRange;
  if (!std::isnan(newYMin) && !std::isnan(newYMax)) {
    m_yRange.setMax(newYMax, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetYMin(newYMin, k_lowerMaxFloat, k_upperMaxFloat);
  }
}

void InteractiveCurveViewRange::setTrigonometric() {
  m_yAuto = false;
  // Set x range
  float x = (Preferences::sharedPreferences()->angleUnit() == Preferences::AngleUnit::Degree) ? 600.0f : 10.5f;
  m_xRange.setMax(x, k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetXMin(-x, k_lowerMaxFloat, k_upperMaxFloat);
  // Set y range
  float y = 1.6f;
  m_yRange.setMax(y, k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetYMin(-y, k_lowerMaxFloat, k_upperMaxFloat);
}

void InteractiveCurveViewRange::setDefault() {
  if (m_delegate == nullptr) {
    return;
  }
  if (!m_delegate->defautRangeIsNormalized()) {
    m_yAuto = true;
    m_xRange.setMax(m_delegate->interestingXHalfRange(), k_lowerMaxFloat, k_upperMaxFloat);
    setXMin(-xMax());
    return;
  }

  m_yAuto = false;
  // Compute the interesting ranges
  float a,b,c,d;
  m_delegate->interestingRanges(&a, &b, &c, &d);
  m_xRange.setMin(a, k_lowerMaxFloat, k_upperMaxFloat);
  m_xRange.setMax(b, k_lowerMaxFloat, k_upperMaxFloat);
  m_yRange.setMin(c, k_lowerMaxFloat, k_upperMaxFloat);
  m_yRange.setMax(d, k_lowerMaxFloat, k_upperMaxFloat);

  // Add margins
  float xRange = xMax() - xMin();
  float yRange = yMax() - yMin();
  m_xRange.setMin(m_delegate->addMargin(xMin(), xRange, false, true), k_lowerMaxFloat, k_upperMaxFloat);
  // Use MemoizedCurveViewRange::protectedSetXMax to update xGridUnit
  MemoizedCurveViewRange::protectedSetXMax(m_delegate->addMargin(xMax(), xRange, false, false), k_lowerMaxFloat, k_upperMaxFloat);
  m_yRange.setMin(m_delegate->addMargin(yMin(), yRange, true, true), k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetYMax(m_delegate->addMargin(yMax(), yRange, true, false), k_lowerMaxFloat, k_upperMaxFloat);

  // Normalize the axes, so that a polar circle is displayed as a circle
  xRange = xMax() - xMin();
  yRange = yMax() - yMin();
  float xyRatio = xRange/yRange;

  const float unit = std::max(xGridUnit(), yGridUnit());
  const float newXHalfRange = NormalizedXHalfRange(unit);
  const float newYHalfRange = NormalizedYHalfRange(unit);
  float normalizedXYRatio = newXHalfRange/newYHalfRange;
  if (xyRatio < normalizedXYRatio) {
    float newXRange = normalizedXYRatio * yRange;
    assert(newXRange > xRange);
    float delta = (newXRange - xRange) / 2.0f;
    m_xRange.setMin(xMin() - delta, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetXMax(xMax()+delta, k_lowerMaxFloat, k_upperMaxFloat);
  } else if (xyRatio > normalizedXYRatio) {
    float newYRange = newYHalfRange/newXHalfRange * xRange;
    assert(newYRange > yRange);
    float delta = (newYRange - yRange) / 2.0f;
    m_yRange.setMin(yMin() - delta, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetYMax(yMax()+delta, k_lowerMaxFloat, k_upperMaxFloat);
  }
}

void InteractiveCurveViewRange::centerAxisAround(Axis axis, float position) {
  if (std::isnan(position)) {
    return;
  }
  if (axis == Axis::X) {
    float range = xMax() - xMin();
    if (std::fabs(position/range) > k_maxRatioPositionRange) {
      range = Range1D::defaultRangeLengthFor(position);
    }
    m_xRange.setMax(position + range/2.0f, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetXMin(position - range/2.0f, k_lowerMaxFloat, k_upperMaxFloat);
  } else {
    m_yAuto = false;
    float range = yMax() - yMin();
    if (std::fabs(position/range) > k_maxRatioPositionRange) {
      range = Range1D::defaultRangeLengthFor(position);
    }
    m_yRange.setMax(position + range/2.0f, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetYMin(position - range/2.0f, k_lowerMaxFloat, k_upperMaxFloat);
  }
}

void InteractiveCurveViewRange::panToMakePointVisible(float x, float y, float topMarginRatio, float rightMarginRatio, float bottomMarginRatio, float leftMarginRatio) {
  if (!std::isinf(x) && !std::isnan(x)) {
    const float xRange = xMax() - xMin();
    const float leftMargin = leftMarginRatio * xRange;
    if (x < xMin() + leftMargin) {
      m_yAuto = false;
      const float newXMin = x - leftMargin;
      m_xRange.setMax(newXMin + xRange, k_lowerMaxFloat, k_upperMaxFloat);
      MemoizedCurveViewRange::protectedSetXMin(newXMin, k_lowerMaxFloat, k_upperMaxFloat);
    }
    const float rightMargin = rightMarginRatio * xRange;
    if (x > xMax() - rightMargin) {
      m_yAuto = false;
      m_xRange.setMax(x + rightMargin, k_lowerMaxFloat, k_upperMaxFloat);
      MemoizedCurveViewRange::protectedSetXMin(xMax() - xRange, k_lowerMaxFloat, k_upperMaxFloat);
    }
  }
  if (!std::isinf(y) && !std::isnan(y)) {
    const float yRange = yMax() - yMin();
    const float bottomMargin = bottomMarginRatio * yRange;
    if (y < yMin() + bottomMargin) {
      m_yAuto = false;
      const float newYMin = y - bottomMargin;
      m_yRange.setMax(newYMin + yRange, k_lowerMaxFloat, k_upperMaxFloat);
      MemoizedCurveViewRange::protectedSetYMin(newYMin, k_lowerMaxFloat, k_upperMaxFloat);
    }
    const float topMargin = topMarginRatio * yRange;
    if (y > yMax() - topMargin) {
      m_yAuto = false;
      m_yRange.setMax(y + topMargin, k_lowerMaxFloat, k_upperMaxFloat);
      MemoizedCurveViewRange::protectedSetYMin(yMax() - yRange, k_lowerMaxFloat, k_upperMaxFloat);
    }
  }
}

void InteractiveCurveViewRange::notifyRangeChange() {
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
}

}
