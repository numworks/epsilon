#include "interactive_curve_view_range.h"
#include <ion.h>
#include <cmath>
#include <stddef.h>
#include <assert.h>
#include <poincare/ieee754.h>
#include <poincare/preferences.h>
#include <poincare/zoom.h>
#include <algorithm>

using namespace Poincare;

namespace Shared {

void InteractiveCurveViewRange::setDelegate(InteractiveCurveViewRangeDelegate * delegate) {
  m_delegate = delegate;
  if (delegate) {
    m_delegate->updateZoomButtons();
  }
}

void InteractiveCurveViewRange::setZoomAuto(bool v) {
  if (m_zoomAuto == v) {
    return;
  }
  m_zoomAuto = v;
  if (m_delegate) {
    m_delegate->updateZoomButtons();
  }
}

void InteractiveCurveViewRange::setZoomNormalize(bool v) {
  if (m_zoomNormalize == v) {
    return;
  }
  m_zoomNormalize = v;
  if (m_delegate) {
    m_delegate->updateZoomButtons();
  }
}

float InteractiveCurveViewRange::roundLimit(float y, float range, bool isMin) {
  /* Floor/ceil to a round number, with a precision depending on the range.
   * A range within : | Will have a magnitude : | 3.14 would be floored to :
   *    [100,1000]    |     10                  | 0
   *    [10,100]      |     1                   | 3
   *    [1,10]        |     0.1                 | 3.1                       */
  float magnitude = std::pow(10.0f, Poincare::IEEE754<float>::exponentBase10(range) - 1.0f);
  if (isMin) {
    return magnitude * std::floor(y / magnitude);
  }
  return magnitude * std::ceil(y / magnitude);
}

void InteractiveCurveViewRange::setXMin(float xMin) {
  MemoizedCurveViewRange::protectedSetXMin(xMin, k_lowerMaxFloat, k_upperMaxFloat);
}

void InteractiveCurveViewRange::setXMax(float xMax) {
  MemoizedCurveViewRange::protectedSetXMax(xMax, k_lowerMaxFloat, k_upperMaxFloat);
}

void InteractiveCurveViewRange::setYMin(float yMin) {
  MemoizedCurveViewRange::protectedSetYMin(yMin, k_lowerMaxFloat, k_upperMaxFloat);
}

void InteractiveCurveViewRange::setYMax(float yMax) {
  MemoizedCurveViewRange::protectedSetYMax(yMax, k_lowerMaxFloat, k_upperMaxFloat);
}

float InteractiveCurveViewRange::yGridUnit() const {
  float res = MemoizedCurveViewRange::yGridUnit();
  if (m_zoomNormalize) {
    /* When m_zoomNormalize is active, both xGridUnit and yGridUnit will be the
     * same. To declutter the X axis, we try a unit twice as large. We check
     * that it allows enough graduations on the Y axis, but if the standard
     * unit would lead to too many graduations on the X axis, we force the
     * larger unit anyways. */
    float numberOfUnits = (yMax() - yMin()) / res;
    if (numberOfUnits > k_maxNumberOfXGridUnits || numberOfUnits / 2.f > k_minNumberOfYGridUnits) {
      return 2 * res;
    }
  }
  return res;
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
  Zoom::SetZoom(ratio, centerX, centerY, &xMi, &xMa, &yMi, &yMa);
  if (!std::isnan(xMi) && !std::isnan(xMa)) {
    setZoomAuto(false);
    m_xRange.setMax(xMa, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetXMin(xMi, k_lowerMaxFloat, k_upperMaxFloat);
  }
  if (!std::isnan(yMi) && !std::isnan(yMa)) {
    setZoomAuto(false);
    m_yRange.setMax(yMa, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetYMin(yMi, k_lowerMaxFloat, k_upperMaxFloat);
  }
  m_offscreenYAxis *= ratio;
  setZoomNormalize(isOrthonormal());
}

void InteractiveCurveViewRange::panWithVector(float x, float y) {
  if (clipped(xMin() + x, false) != xMin() + x || clipped(xMax() + x, true) != xMax() + x || clipped(yMin() + y, false) != yMin() + y || clipped(yMax() + y, true) != yMax() + y || std::isnan(clipped(xMin() + x, false)) || std::isnan(clipped(xMax() + x, true)) || std::isnan(clipped(yMin() + y, false)) || std::isnan(clipped(yMax() + y, true))) {
    return;
  }
  if (x != 0.f || y != 0.f) {
    setZoomAuto(false);
  }
  m_xRange.setMax(xMax()+x, k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetXMin(xMin() + x, k_lowerMaxFloat, k_upperMaxFloat);
  m_yRange.setMax(yMax()+y, k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetYMin(yMin() + y, k_lowerMaxFloat, k_upperMaxFloat);
}

void InteractiveCurveViewRange::normalize(bool forceChangeY) {
  /* We center the ranges on the current range center, and put each axis so that
   * 1cm = 2 current units. */

  if (isOrthonormal()) {
    return;
  }

  setZoomAuto(false);

  float newXMin = xMin(), newXMax = xMax(), newYMin = yMin(), newYMax = yMax();

  const float unit = std::max(xGridUnit(), yGridUnit());
  const float newXHalfRange = NormalizedXHalfRange(unit);
  const float newYHalfRange = NormalizedYHalfRange(unit);
  float normalizedYXRatio = newYHalfRange/newXHalfRange;

  /* Most of the time, we do not want to shrink, to avoid hiding parts of the
   * function. However, when forceChangeY is true, we shrink if the Y range is
   * the longer one. */
  bool shrink = forceChangeY && (newYMax - newYMin) / (newXMax - newXMin) > normalizedYXRatio;
  Zoom::SetToRatio(normalizedYXRatio, &newXMin, &newXMax, &newYMin, &newYMax, shrink);

  m_xRange.setMin(newXMin, k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetXMax(newXMax, k_lowerMaxFloat, k_upperMaxFloat);
  m_yRange.setMin(newYMin, k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetYMax(newYMax, k_lowerMaxFloat, k_upperMaxFloat);

  /* The range should be close to orthonormal, unless :
   *   - it has been clipped because the maximum bounds have been reached.
   *   - the the bounds are too close and of too large a magnitude, leading to
   *     a drastic loss of significance. */
  assert(isOrthonormal()
      || xMin() <= - k_lowerMaxFloat || xMax() >= k_lowerMaxFloat || yMin() <= - k_lowerMaxFloat || yMax() >= k_lowerMaxFloat
      || normalizationSignificantBits() <= 0);
  setZoomNormalize(isOrthonormal());
}

void InteractiveCurveViewRange::setDefault() {
  if (m_delegate == nullptr) {
    return;
  }

  /* If m_zoomNormalize was left active, xGridUnit() would return the value of
   * yGridUnit, even if the ranger were not truly normalized. We use
   * setZoomNormalize to refresh the button in case the graph does not end up
   * normalized. */
  setZoomNormalize(false);

  // Compute the interesting range
  m_delegate->interestingRanges(this);
  /* If the horizontal bounds are integers, they are preset values and should
   * not be changed. */
  bool isDefaultRange = hasDefaultRange();

  // Add margins, then round limits.
  float newXMin = xMin(), newXMax = xMax();
  if (!isDefaultRange) {
    float xRange = xMax() - xMin();
    newXMin = roundLimit(m_delegate->addMargin(xMin(), xRange, false, true), xRange, true);
    newXMax = roundLimit(m_delegate->addMargin(xMax(), xRange, false, false), xRange, false);
  }
  m_xRange.setMin(newXMin, k_lowerMaxFloat, k_upperMaxFloat);
  // Use MemoizedCurveViewRange::protectedSetXMax to update xGridUnit
  MemoizedCurveViewRange::protectedSetXMax(newXMax, k_lowerMaxFloat, k_upperMaxFloat);
  float yRange = yMax() - yMin();
  m_yRange.setMin(roundLimit(m_delegate->addMargin(yMin(), yRange, true , true), yRange, true), k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetYMax(roundLimit(m_delegate->addMargin(yMax(), yRange, true , false), yRange, false), k_lowerMaxFloat, k_upperMaxFloat);

  if (m_delegate->defaultRangeIsNormalized() || shouldBeNormalized()) {
    /* Normalize the axes, so that a polar circle is displayed as a circle.
     * If we are displaying cartesian functions with a default range, we want
     * the X bounds untouched. */
    normalize(isDefaultRange && !m_delegate->defaultRangeIsNormalized());
  }

  setZoomAuto(true);
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
    float newXMax = position + range/2.0f;
    if (xMax() != newXMax) {
      setZoomAuto(false);
      m_xRange.setMax(newXMax, k_lowerMaxFloat, k_upperMaxFloat);
      MemoizedCurveViewRange::protectedSetXMin(newXMax - range, k_lowerMaxFloat, k_upperMaxFloat);
    }
  } else {
    float range = yMax() - yMin();
    if (std::fabs(position/range) > k_maxRatioPositionRange) {
      range = Range1D::defaultRangeLengthFor(position);
    }
    float newYMax = position + range/2.0f;
    if (yMax() != newYMax) {
      setZoomAuto(false);
      m_yRange.setMax(position + range/2.0f, k_lowerMaxFloat, k_upperMaxFloat);
      MemoizedCurveViewRange::protectedSetYMin(position - range/2.0f, k_lowerMaxFloat, k_upperMaxFloat);
    }
  }

  setZoomNormalize(isOrthonormal());
}

void InteractiveCurveViewRange::panToMakePointVisible(float x, float y, float topMarginRatio, float rightMarginRatio, float bottomMarginRatio, float leftMarginRatio, float pixelWidth) {
  if (!std::isinf(x) && !std::isnan(x)) {
    const float xRange = xMax() - xMin();
    const float leftMargin = leftMarginRatio * xRange;
    if (x < xMin() + leftMargin) {
      setZoomAuto(false);
      /* The panning increment is a whole number of pixels so that the caching
       * for cartesian functions is not invalidated. */
      const float newXMin = std::floor((x - leftMargin - xMin()) / pixelWidth) * pixelWidth + xMin();
      m_xRange.setMax(newXMin + xRange, k_lowerMaxFloat, k_upperMaxFloat);
      MemoizedCurveViewRange::protectedSetXMin(newXMin, k_lowerMaxFloat, k_upperMaxFloat);
    }
    const float rightMargin = rightMarginRatio * xRange;
    if (x > xMax() - rightMargin) {
      setZoomAuto(false);
      const float newXMax = std::ceil((x + rightMargin - xMax()) / pixelWidth) * pixelWidth + xMax();
      m_xRange.setMax(newXMax, k_lowerMaxFloat, k_upperMaxFloat);
      MemoizedCurveViewRange::protectedSetXMin(xMax() - xRange, k_lowerMaxFloat, k_upperMaxFloat);
    }
  }
  if (!std::isinf(y) && !std::isnan(y)) {
    const float yRange = yMax() - yMin();
    const float bottomMargin = bottomMarginRatio * yRange;
    if (y < yMin() + bottomMargin) {
      setZoomAuto(false);
      const float newYMin = y - bottomMargin;
      m_yRange.setMax(newYMin + yRange, k_lowerMaxFloat, k_upperMaxFloat);
      MemoizedCurveViewRange::protectedSetYMin(newYMin, k_lowerMaxFloat, k_upperMaxFloat);
    }
    const float topMargin = topMarginRatio * yRange;
    if (y > yMax() - topMargin) {
      setZoomAuto(false);
      m_yRange.setMax(y + topMargin, k_lowerMaxFloat, k_upperMaxFloat);
      MemoizedCurveViewRange::protectedSetYMin(yMax() - yRange, k_lowerMaxFloat, k_upperMaxFloat);
    }
  }

  /* Panning to a point greater than the maximum range of 10^8 could make the
   * graph not normalized.*/
  setZoomNormalize(isOrthonormal());
}

bool InteractiveCurveViewRange::shouldBeNormalized() const {
  float ratio = (yMax() - yMin()) / (xMax() - xMin());
  return ratio >= NormalYXRatio() / k_orthonormalTolerance && ratio <= NormalYXRatio() * k_orthonormalTolerance;
}

bool InteractiveCurveViewRange::isOrthonormal() const {
  int significantBits = normalizationSignificantBits();
  if (significantBits <= 0) {
    return false;
  }
  float ratio = (yMax() - yMin() + offscreenYAxis()) / (xMax() - xMin());
  /* The last N (= 23 - significantBits) bits of "ratio" mantissa have become
   * insignificant. "tolerance" is the difference between ratio with those N
   * bits set to 1, and ratio with those N bits set to 0 ; i.e. a measure of
   * the interval in which numbers are indistinguishable from ratio with this
   * level of precision. */
  float tolerance = std::pow(2.f, IEEE754<float>::exponent(ratio) - significantBits);
  return  ratio - tolerance <= NormalYXRatio() && ratio + tolerance >= NormalYXRatio();
}

int InteractiveCurveViewRange::normalizationSignificantBits() const {
    float xr = std::fabs(xMin()) > std::fabs(xMax()) ? xMax() / xMin() : xMin() / xMax();
    float yr = std::fabs(yMin()) > std::fabs(yMax()) ? yMax() / yMin() : yMin() / yMax();
    /* The subtraction x - y induces a loss of significance of -log2(1-x/y)
     * bits. Since normalizing requires computing xMax - xMin and yMax - yMin,
     * the ratio of the normalized range will deviate from the Normal ratio. We
     * add an extra two lost bits to account for loss of precision from other
     * sources. */
    float loss = std::log2(std::min(1.f - xr, 1.f - yr));
    if (loss > 0.f) {
      loss = 0.f;
    }
    return  std::floor(loss + 23.f - 2.f);
}

}
