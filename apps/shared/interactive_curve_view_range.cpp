#include "interactive_curve_view_range.h"
#include <ion.h>
#include <cmath>
#include <stddef.h>
#include <assert.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/ieee754.h>
#include <poincare/preferences.h>
#include <poincare/zoom.h>
#include <algorithm>

using namespace Poincare;

namespace Shared {

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

void InteractiveCurveViewRange::setDelegate(InteractiveCurveViewRangeDelegate * delegate) {
  m_delegate = delegate;
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
  assert(!xAuto() || m_delegate == nullptr);
  MemoizedCurveViewRange::protectedSetXMin(xMin, true, k_maxFloat);
  m_yMinIntrinsic = NAN;
  m_yMaxIntrinsic = NAN;
  computeRanges();
}

void InteractiveCurveViewRange::setXMax(float xMax) {
  assert(!xAuto() || m_delegate == nullptr);
  MemoizedCurveViewRange::protectedSetXMax(xMax, true, k_maxFloat);
  m_yMinIntrinsic = NAN;
  m_yMaxIntrinsic = NAN;
  computeRanges();
}

void InteractiveCurveViewRange::setYMin(float yMin) {
  assert(!yAuto() || m_delegate == nullptr);
  MemoizedCurveViewRange::protectedSetYMin(yMin, true, k_maxFloat);
  setZoomNormalize(isOrthonormal());
}

void InteractiveCurveViewRange::setYMax(float yMax) {
  assert(!yAuto() || m_delegate == nullptr);
  MemoizedCurveViewRange::protectedSetYMax(yMax, true, k_maxFloat);
  setZoomNormalize(isOrthonormal());
}

void InteractiveCurveViewRange::setOffscreenYAxis(float f) {
  float d = m_offscreenYAxis - f;
  m_offscreenYAxis = f;
  MemoizedCurveViewRange::protectedSetYMax(yMax() + d, true, k_maxFloat);
}

float InteractiveCurveViewRange::yGridUnit() const {
  float res = MemoizedCurveViewRange::yGridUnit();
  if (m_zoomNormalize) {
    /* When m_zoomNormalize is active, both xGridUnit and yGridUnit will be the
     * same. To declutter the X axis, we try a unit twice as large. We check
     * that it allows enough graduations on the Y axis, but if the standard
     * unit would lead to too many graduations on the X axis, we force the
     * larger unit anyways. */
    float numberOfUnits = (yMax() - yMin() + offscreenYAxis()) / res;
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
  if (ratio*std::fabs(xMa-xMi) < Range1D::k_minLength || ratio*std::fabs(yMa-yMi) < Range1D::k_minLength) {
    return;
  }
  float centerX = std::isnan(x) || std::isinf(x) ? xCenter() : x;
  float centerY = std::isnan(y) || std::isinf(y) ? yCenter() : y;
  Zoom::SetZoom(ratio, centerX, centerY, &xMi, &xMa, &yMi, &yMa);
  if (!std::isnan(xMi) && !std::isnan(xMa)) {
    setZoomAuto(false);
    MemoizedCurveViewRange::protectedSetXMax(xMa, false, k_maxFloat);
    MemoizedCurveViewRange::protectedSetXMin(xMi, true, k_maxFloat);
  }
  if (!std::isnan(yMi) && !std::isnan(yMa)) {
    setZoomAuto(false);
    float yMaxOld = yMax(), yMinOld = yMin();
    MemoizedCurveViewRange::protectedSetYMax(yMa, false, k_maxFloat);
    MemoizedCurveViewRange::protectedSetYMin(yMi, true, k_maxFloat);
    /* The factor will typically be equal to ratio, unless yMax and yMin are
     * close to the maximal values. */
    m_offscreenYAxis *= (yMax() - yMin()) / (yMaxOld - yMinOld);
  }
  setZoomNormalize(isOrthonormal());
}

void InteractiveCurveViewRange::panWithVector(float x, float y) {
  Range1D xRange(xMin(), xMax());
  xRange.setMin(xMin() + x);
  xRange.setMax(xMax() + x);
  Range1D yRange(yMin(), yMax());
  yRange.setMin(yMin() + y);
  yRange.setMax(yMax() + y);
  if (xRange.min() != xMin() + x || xRange.max() != xMax() + x || yRange.min() != yMin() + x || yRange.max() != yMax() + x) {
    return;
  }

  if (x != 0.f || y != 0.f) {
    setZoomAuto(false);
  }
  MemoizedCurveViewRange::protectedSetXMax(xMax() + x, false, k_maxFloat);
  MemoizedCurveViewRange::protectedSetXMin(xMin() + x, k_maxFloat);
  MemoizedCurveViewRange::protectedSetYMax(yMax() + y, false, k_maxFloat);
  MemoizedCurveViewRange::protectedSetYMin(yMin() + y, k_maxFloat);
}

void InteractiveCurveViewRange::normalize() {
  m_yMinIntrinsic = NAN;
  m_yMaxIntrinsic = NAN;
  /* If one axis is set manually and the other is in auto mode, prioritize
   * changing the auto one. */
  bool canChangeX = m_xAuto || !m_yAuto;
  bool canChangeY = m_yAuto || !m_xAuto;
  setZoomAuto(false);
  protectedNormalize(canChangeX, canChangeY, !canChangeX || !canChangeY);
}

void InteractiveCurveViewRange::centerAxisAround(Axis axis, float position) {
  if (std::isnan(position)) {
    return;
  }
  if (axis == Axis::X) {
    float range = xMax() - xMin();
    if (std::fabs(position/range) > k_maxRatioPositionRange) {
      range = Range1D::DefaultLengthAt(position);
    }
    float newXMax = position + range/2.0f;
    if (xMax() != newXMax) {
      setZoomAuto(false);
      MemoizedCurveViewRange::protectedSetXMax(newXMax, false, k_maxFloat);
      MemoizedCurveViewRange::protectedSetXMin(newXMax - range, true, k_maxFloat);
    }
  } else {
    float range = yMax() - yMin();
    if (std::fabs(position/range) > k_maxRatioPositionRange) {
      range = Range1D::DefaultLengthAt(position);
    }
    float newYMax = position + range/2.0f;
    if (yMax() != newYMax) {
      setZoomAuto(false);
      MemoizedCurveViewRange::protectedSetYMax(position + range/2.0f, false, k_maxFloat);
      MemoizedCurveViewRange::protectedSetYMin(position - range/2.0f, true, k_maxFloat);
    }
  }

  setZoomNormalize(isOrthonormal());
}

bool InteractiveCurveViewRange::panToMakePointVisible(float x, float y, float topMarginRatio, float rightMarginRatio, float bottomMarginRatio, float leftMarginRatio, float pixelWidth) {
  bool moved = false;
  if (!std::isinf(x) && !std::isnan(x)) {
    const float xRange = xMax() - xMin();
    const float leftMargin = leftMarginRatio * xRange;
    if (x < xMin() + leftMargin) {
      moved = true;
      /* The panning increment is a whole number of pixels so that the caching
       * for cartesian functions is not invalidated. */
      const float newXMin = std::floor((x - leftMargin - xMin()) / pixelWidth) * pixelWidth + xMin();
      MemoizedCurveViewRange::protectedSetXMax(newXMin + xRange, false, k_maxFloat);
      MemoizedCurveViewRange::protectedSetXMin(newXMin, true, k_maxFloat);
    }
    const float rightMargin = rightMarginRatio * xRange;
    if (x > xMax() - rightMargin) {
      moved = true;
      const float newXMax = std::ceil((x + rightMargin - xMax()) / pixelWidth) * pixelWidth + xMax();
      MemoizedCurveViewRange::protectedSetXMax(newXMax, false, k_maxFloat);
      MemoizedCurveViewRange::protectedSetXMin(xMax() - xRange, true, k_maxFloat);
    }
  }
  if (!std::isinf(y) && !std::isnan(y)) {
    const float yRange = yMax() - yMin();
    const float bottomMargin = bottomMarginRatio * yRange;
    if (y < yMin() + bottomMargin) {
      moved = true;
      const float newYMin = y - bottomMargin;
      MemoizedCurveViewRange::protectedSetYMax(newYMin + yRange, false, k_maxFloat);
      MemoizedCurveViewRange::protectedSetYMin(newYMin, true, k_maxFloat);
    }
    const float topMargin = topMarginRatio * yRange;
    if (y > yMax() - topMargin) {
      moved = true;
      MemoizedCurveViewRange::protectedSetYMax(y + topMargin, false, k_maxFloat);
      MemoizedCurveViewRange::protectedSetYMin(yMax() - yRange, true, k_maxFloat);
    }
  }

  if (moved) {
    setZoomAuto(false);
  }

  /* Panning to a point greater than the maximum range of 10^8 could make the
   * graph not normalized.*/
  setZoomNormalize(isOrthonormal());

  return moved;
}

static void ComputeNewBoundsAfterZoomingOut(float x, float minBoundWithMargin, float maxBoundWithMargin, float minBoundMarginRatio, float maxBoundMarginRatio, float * newMinBound, float * newMaxBound) {
  // One of the bounds within margins becomes x
  if (x < minBoundWithMargin) {
    minBoundWithMargin = x;
  } else {
    assert(x > maxBoundWithMargin);
    maxBoundWithMargin = x;
  }
  /* When we zoom out, we want to recompute both the xMin and xMax so that
   * previous values that where within margins bounds stay in it, even if
   * the xRange increased.
   *
   * |-------------|----------------------------|------------|
   * ^newMinBound (X)                                        ^newMaxBound (Y)
   *               ^minBoundwithMargin (A)      ^maxBoundwithMargin (B)
   *        ^minMarginRatio (r)                       ^maxMarginRatio (R)
   *
   * We have to solve the equation system:
   * maxMargin = maxMarginRatio * (newMaxBound - newMinBound)
   * minMargin = minMarginRation * (newMaxBound - newMinBound)
   * which is equivalent to:
   * Y - B = R * (Y - X)
   * A - X = r * (Y - X)
   *
   * We find the formulas:
   * X = A - (r * (B - A) / (1 - (R + r)))
   * Y = B + (R * (B - A) / (1 - (R + r)))
   */
  assert(maxBoundMarginRatio < 0.5 && minBoundMarginRatio < 0.5);
  *newMinBound = minBoundWithMargin - (minBoundMarginRatio * (maxBoundWithMargin - minBoundWithMargin) / (1 - (maxBoundMarginRatio + minBoundMarginRatio)));
  *newMaxBound = maxBoundWithMargin + (maxBoundMarginRatio * (maxBoundWithMargin - minBoundWithMargin) / (1 - (maxBoundMarginRatio + minBoundMarginRatio)));
}

bool InteractiveCurveViewRange::zoomOutToMakePointVisible(float x, float y, float topMarginRatio, float rightMarginRatio, float bottomMarginRatio, float leftMarginRatio) {
  bool moved = false;
  if (!std::isinf(x) && !std::isnan(x)) {
    const float xRange = xMax() - xMin();
    const float xMinWithMargin = xMin() + leftMarginRatio * xRange;
    const float xMaxWithMargin = xMax() - rightMarginRatio * xRange;
    if (x < xMinWithMargin || x > xMaxWithMargin) {
      moved = true;
      float newXMin, newXMax;
      ComputeNewBoundsAfterZoomingOut(x, xMinWithMargin, xMaxWithMargin, leftMarginRatio, rightMarginRatio, &newXMin, &newXMax);
      MemoizedCurveViewRange::protectedSetXMax(newXMax, true, k_maxFloat);
      MemoizedCurveViewRange::protectedSetXMin(newXMin, true, k_maxFloat);
    }
  }
  if (!std::isinf(y) && !std::isnan(y)) {
    const float yRange = yMax() - yMin();
    const float yMinWithMargin = yMin() + bottomMarginRatio * yRange;
    const float yMaxWithMargin = yMax() - topMarginRatio * yRange;
    if (y < yMinWithMargin || y > yMaxWithMargin) {
      moved = true;
      float newYMin, newYMax;
      ComputeNewBoundsAfterZoomingOut(y, yMinWithMargin, yMaxWithMargin, bottomMarginRatio, topMarginRatio, &newYMin, &newYMax);
      MemoizedCurveViewRange::protectedSetYMax(newYMax, true, k_maxFloat);
      MemoizedCurveViewRange::protectedSetYMin(newYMin, true, k_maxFloat);
    }
  }

  if (moved) {
    setZoomAuto(false);
  }

  /* Zomming out to a point greater than the maximum range of 10^8 could make the
   * graph not normalized.*/
  setZoomNormalize(isOrthonormal());

  return moved;
}

bool InteractiveCurveViewRange::shouldBeNormalized() const {
  float ratio = (yMax() - yMin()) / (xMax() - xMin());
  return ratio >= NormalYXRatio() / k_orthonormalTolerance && ratio <= NormalYXRatio() * k_orthonormalTolerance;
}

void InteractiveCurveViewRange::protectedNormalize(bool canChangeX, bool canChangeY, bool canShrink) {
  /* We center the ranges on the current range center, and put each axis so that
   * 1cm = 2 current units. */

  if (isOrthonormal() || !(canChangeX || canChangeY)) {
    setZoomNormalize(isOrthonormal());
    return;
  }

  float newXMin = xMin(), newXMax = xMax(), newYMin = yMin(), newYMax = yMax();

  const float unit = std::max(xGridUnit(), yGridUnit());
  const float newXHalfRange = NormalizedXHalfRange(unit);
  const float newYHalfRange = NormalizedYHalfRange(unit);
  float normalizedYXRatio = newYHalfRange/newXHalfRange;

  /* We try to normalize by expanding instead of shrinking as much as possible,
   * since shrinking can hide parts of the curve. If the axis we would like to
   * expand cannot be changed, we shrink the other axis instead, if allowed.
   * If the Y axis is too long, shrink Y if you cannot extend X ; but if the Y
   * axis is too short, shrink X if you cannot extend X. */
  bool shrink = (newYMax - newYMin) / (newXMax - newXMin) > normalizedYXRatio ? !canChangeX : !canChangeY;

  if (!shrink || canShrink) {
    Zoom::SetToRatio(normalizedYXRatio, &newXMin, &newXMax, &newYMin, &newYMax, shrink);

    MemoizedCurveViewRange::protectedSetXMin(newXMin, false, k_maxFloat);
    MemoizedCurveViewRange::protectedSetXMax(newXMax, true, k_maxFloat);
    MemoizedCurveViewRange::protectedSetYMin(newYMin, false, k_maxFloat);
    MemoizedCurveViewRange::protectedSetYMax(newYMax, true, k_maxFloat);

    /* The range should be close to orthonormal, unless :
     *   - it has been clipped because the maximum bounds have been reached.
     *   - the the bounds are too close and of too large a magnitude, leading to
     *     a drastic loss of significance. */
    assert(isOrthonormal()
        || xMin() <= - k_maxFloat || xMax() >= k_maxFloat || yMin() <= - k_maxFloat || yMax() >= k_maxFloat
        || normalizationSignificantBits() <= 0);
  }
  setZoomNormalize(isOrthonormal());
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

void InteractiveCurveViewRange::privateSetZoomAuto(bool xAuto, bool yAuto) {
  bool oldAuto = zoomAuto();
  m_xAuto = xAuto;
  m_yAuto = yAuto;
  if (m_delegate && (oldAuto != zoomAuto())) {
    m_delegate->updateZoomButtons();
  }
}

void InteractiveCurveViewRange::privateComputeRanges(bool computeX, bool computeY) {
  if (m_delegate == nullptr) {
    return;
  }

  if (offscreenYAxis() != 0.f) {
    /* The Navigation window was exited without being cleaned up, probably
     * because the User pressed the Home button.
     * We reset the value here to prevent skewing the grid unit. */
    setOffscreenYAxis(0.f);
  }

  /* If m_zoomNormalize was left active, xGridUnit() would return the value of
   * yGridUnit, even if the range were not truly normalized. We use
   * setZoomNormalize to refresh the button in case the graph does not end up
   * normalized. */
  setZoomNormalize(false);

  float newXMin, newXMax, newYMin, newYMax;

  Poincare::UserCircuitBreakerCheckpoint checkpoint;
  if (CircuitBreakerRun(checkpoint)) {
    if (computeX || intrinsicYRangeIsUnset()) {
      float xMinLimit, xMaxLimit;
      if (computeX) {
        xMinLimit = -INFINITY;
        xMaxLimit = INFINITY;
      } else {
        xMinLimit = xMin();
        xMaxLimit = xMax();
      }
      m_delegate->computeXRange(xMinLimit, xMaxLimit, &newXMin, &newXMax, &m_yMinIntrinsic, &m_yMaxIntrinsic);
    }
    if (computeX) {
      // No need to update xGridUnit beffor having set XMax.
      MemoizedCurveViewRange::protectedSetXMin(newXMin, false, k_maxFloat);
      MemoizedCurveViewRange::protectedSetXMax(newXMax, true, k_maxFloat);
      if (!hasDefaultRange()) {
        float dx = xMax() - xMin();
        MemoizedCurveViewRange::protectedSetXMin(roundLimit(m_delegate->addMargin(newXMin, dx, false , true), dx, true),  false, k_maxFloat);
        MemoizedCurveViewRange::protectedSetXMax(roundLimit(m_delegate->addMargin(newXMax, dx, false , false), dx, false), true, k_maxFloat);
      }
    }

    /* We notify the delegate to refresh the cursor's position and update the
     * bottom margin (which depends on the banner height). */
    m_delegate->updateBottomMargin();

    float dy;
    if (computeY || (computeX && m_yAuto)) {
      assert(!intrinsicYRangeIsUnset());
      /* If X and Y are computed automatically, we want the Y range to be
       * fitted to the curve. Otherwise, we display the full Y range. */
      m_delegate->computeYRange(xMin(), xMax(), m_yMinIntrinsic, m_yMaxIntrinsic, &newYMin, &newYMax, m_xAuto && m_yAuto);
      if (computeX) {
        newXMin = xMin();
        newXMax = xMax();
        m_delegate->improveFullRange(&newXMin, &newXMax, &newYMin, &newYMax);
        MemoizedCurveViewRange::protectedSetXMin(newXMin, false, k_maxFloat);
        MemoizedCurveViewRange::protectedSetXMax(newXMax, true, k_maxFloat);
      }
      /* Add vertical margins */
      dy = newYMax - newYMin;
      MemoizedCurveViewRange::protectedSetYMin(roundLimit(m_delegate->addMargin(newYMin, dy, true , true), dy, true), false, k_maxFloat);
      MemoizedCurveViewRange::protectedSetYMax(roundLimit(m_delegate->addMargin(newYMax, dy, true , false), dy, false), true, k_maxFloat);
    }

    if (m_delegate->defaultRangeIsNormalized() || shouldBeNormalized()) {
      /* Normalize the axes, so that a polar circle is displayed as a circle.
       * If we are displaying cartesian functions, we want the X bounds
       * untouched. */
      bool shrink = m_delegate->canShrinkWhenNormalizing();
      bool canChangeX = computeX && (m_delegate->defaultRangeIsNormalized() || !shrink);
      protectedNormalize(canChangeX, computeY, shrink);
      if (computeY && !intrinsicYRangeIsUnset() && m_yMinIntrinsic <= m_yMaxIntrinsic) {
        /* Make sure the intrinsic Y values are on screen. */
        float yMinIntrinsicWithMargin = m_delegate->addMargin(m_yMinIntrinsic, dy, true, true);
        bool changedYMin = false;
        if (yMinIntrinsicWithMargin < yMin()) {
          float delta = yMin() - yMinIntrinsicWithMargin;
          MemoizedCurveViewRange::protectedSetYMin(yMinIntrinsicWithMargin, false, k_maxFloat);
          MemoizedCurveViewRange::protectedSetYMax(yMax() - delta, true, k_maxFloat);
          changedYMin = true;
        }
        float yMaxIntrinsicWithMargin = m_delegate->addMargin(m_yMaxIntrinsic, dy, true, false);
        if (yMaxIntrinsicWithMargin > yMax()) {
          float delta = yMaxIntrinsicWithMargin - yMax();
          MemoizedCurveViewRange::protectedSetYMax(yMaxIntrinsicWithMargin, true, k_maxFloat);
          if (!changedYMin && yMinIntrinsicWithMargin >= yMin() + delta) {
            MemoizedCurveViewRange::protectedSetYMin(yMin() + delta, true, k_maxFloat);
          }
        }
        setZoomNormalize(isOrthonormal());
      }
    }
  } else {
    m_delegate->tidyModels();

    float xMin = -10.f;
    float xMax = 10.f;
    MemoizedCurveViewRange::protectedSetXMin(xMin, false, k_maxFloat);
    MemoizedCurveViewRange::protectedSetXMax(xMax, true, k_maxFloat);
    float yMin, yMax;
    Poincare::Zoom::SanitizeRangeForDisplay(&yMin, &yMax, NormalYXRatio() * (xMax - xMin) / 2.f);
    MemoizedCurveViewRange::protectedSetYMin(yMin, false, k_maxFloat);
    MemoizedCurveViewRange::protectedSetYMax(yMax, true, k_maxFloat);
  }
}

}
