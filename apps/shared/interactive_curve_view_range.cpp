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
  float data[] = {xMin(), xMax(), yMin(), yMax()};
  size_t dataLengthInBytes = sizeof(data);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  return Ion::crc32Word((uint32_t *)data, dataLengthInBytes/sizeof(uint32_t));
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
  if (clipped(xMin() + x, false) != xMin() + x || clipped(xMax() + x, true) != xMax() + x || clipped(yMin() + y, false) != yMin() + y || clipped(yMax() + y, true) != yMax() + y || std::isnan(clipped(xMin() + x, false)) || std::isnan(clipped(xMax() + x, true)) || std::isnan(clipped(yMin() + y, false)) || std::isnan(clipped(yMax() + y, true))) {
    return;
  }
  m_xRange.setMax(xMax()+x, k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetXMin(xMin() + x, k_lowerMaxFloat, k_upperMaxFloat);
  m_yRange.setMax(yMax()+y, k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetYMin(yMin() + y, k_lowerMaxFloat, k_upperMaxFloat);
}

void InteractiveCurveViewRange::normalize() {
  /* We center the ranges on the current range center, and put each axis so that
   * 1cm = 2 current units. */

  float xRange = xMax() - xMin();
  float yRange = yMax() - yMin();
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

void InteractiveCurveViewRange::setDefault() {
  if (m_delegate == nullptr) {
    return;
  }

  // Compute the interesting range
  m_delegate->interestingRanges(this);

  // Add margins
  float xRange = xMax() - xMin();
  float yRange = yMax() - yMin();
  m_xRange.setMin(m_delegate->addMargin(xMin(), xRange, false, true), k_lowerMaxFloat, k_upperMaxFloat);
  // Use MemoizedCurveViewRange::protectedSetXMax to update xGridUnit
  MemoizedCurveViewRange::protectedSetXMax(m_delegate->addMargin(xMax(), xRange, false, false), k_lowerMaxFloat, k_upperMaxFloat);
  m_yRange.setMin(m_delegate->addMargin(yMin(), yRange, true, true), k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetYMax(m_delegate->addMargin(yMax(), yRange, true, false), k_lowerMaxFloat, k_upperMaxFloat);

  if (!m_delegate->defaultRangeIsNormalized()) {
    return;
  }

  // Normalize the axes, so that a polar circle is displayed as a circle
  normalize();
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
    float range = yMax() - yMin();
    if (std::fabs(position/range) > k_maxRatioPositionRange) {
      range = Range1D::defaultRangeLengthFor(position);
    }
    m_yRange.setMax(position + range/2.0f, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetYMin(position - range/2.0f, k_lowerMaxFloat, k_upperMaxFloat);
  }
}

void InteractiveCurveViewRange::panToMakePointVisible(float x, float y, float topMarginRatio, float rightMarginRatio, float bottomMarginRatio, float leftMarginRatio, float pixelWidth) {
  if (!std::isinf(x) && !std::isnan(x)) {
    const float xRange = xMax() - xMin();
    const float leftMargin = leftMarginRatio * xRange;
    if (x < xMin() + leftMargin) {
      /* The panning increment is a whole number of pixels so that the caching
       * for cartesian functions is not invalidated. */
      const float newXMin = std::floor((x - leftMargin - xMin()) / pixelWidth) * pixelWidth + xMin();
      m_xRange.setMax(newXMin + xRange, k_lowerMaxFloat, k_upperMaxFloat);
      MemoizedCurveViewRange::protectedSetXMin(newXMin, k_lowerMaxFloat, k_upperMaxFloat);
    }
    const float rightMargin = rightMarginRatio * xRange;
    if (x > xMax() - rightMargin) {
      const float newXMax = std::ceil((x + rightMargin - xMax()) / pixelWidth) * pixelWidth + xMax();
      m_xRange.setMax(newXMax, k_lowerMaxFloat, k_upperMaxFloat);
      MemoizedCurveViewRange::protectedSetXMin(xMax() - xRange, k_lowerMaxFloat, k_upperMaxFloat);
    }
  }
  if (!std::isinf(y) && !std::isnan(y)) {
    const float yRange = yMax() - yMin();
    const float bottomMargin = bottomMarginRatio * yRange;
    if (y < yMin() + bottomMargin) {
      const float newYMin = y - bottomMargin;
      m_yRange.setMax(newYMin + yRange, k_lowerMaxFloat, k_upperMaxFloat);
      MemoizedCurveViewRange::protectedSetYMin(newYMin, k_lowerMaxFloat, k_upperMaxFloat);
    }
    const float topMargin = topMarginRatio * yRange;
    if (y > yMax() - topMargin) {
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
