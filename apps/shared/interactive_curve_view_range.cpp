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
#include <math.h>

using namespace Poincare;

namespace Shared {

bool InteractiveCurveViewRange::isOrthonormal() const {
  Range2D range = memoizedRange();
  range = Range2D(*range.x(), Range1D(range.yMin(), range.yMax() + offscreenYAxis()));
  return range.ratioIs(NormalYXRatio());
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
  computeRanges();
}

void InteractiveCurveViewRange::setXMax(float xMax) {
  assert(!xAuto() || m_delegate == nullptr);
  MemoizedCurveViewRange::protectedSetXMax(xMax, true, k_maxFloat);
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
  Range2D thisRange = memoizedRange();
  float dy = thisRange.y()->length();
  if (ratio * std::min(thisRange.x()->length(), dy) < Range1D::k_minLength) {
    return;
  }
  Coordinate2D<float> center(std::isfinite(x) ? x : thisRange.x()->center(), std::isfinite(y) ? y : thisRange.y()->center());
  thisRange.zoom(ratio, center);
  assert(thisRange.x()->isValid() && thisRange.y()->isValid());
  setZoomAuto(false);
  protectedSetX(*thisRange.x(), k_maxFloat);
  protectedSetY(*thisRange.y(), k_maxFloat);
  /* The factor will typically be equal to ratio, unless yMax and yMin are
   * close to the maximal values. */
  float yRatio = memoizedRange().y()->length() / dy;
  m_offscreenYAxis *= yRatio;
  setZoomNormalize(isOrthonormal());
}

void InteractiveCurveViewRange::panWithVector(float x, float y) {
  Range1D xRange(xMin(), xMax());
  xRange.setMin(xMin() + x, k_maxFloat);
  xRange.setMax(xMax() + x, k_maxFloat);
  if (xRange.min() != xMin() + x || xRange.max() != xMax() + x) {
    return;
  }

  Range1D yRange(yMin(), yMax());
  yRange.setMin(yMin() + y, k_maxFloat);
  yRange.setMax(yMax() + y, k_maxFloat);
  if (yRange.min() != yMin() + y || yRange.max() != yMax() + y) {
    return;
  }

  if (x != 0.f || y != 0.f) {
    setZoomAuto(false);
  }
  protectedSetX(xRange, k_maxFloat);
  protectedSetY(yRange, k_maxFloat);
}

void InteractiveCurveViewRange::normalize() {
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
      protectedSetX(Range1D(newXMax - range, newXMax), k_maxFloat);
    }
  } else {
    float range = yMax() - yMin();
    if (std::fabs(position/range) > k_maxRatioPositionRange) {
      range = Range1D::DefaultLengthAt(position);
    }
    float newYMax = position + range/2.0f;
    if (yMax() != newYMax) {
      setZoomAuto(false);
      protectedSetY(Range1D(position - 0.5f * range, position + 0.5f * range), k_maxFloat);
    }
  }

  setZoomNormalize(isOrthonormal());
}

bool InteractiveCurveViewRange::panToMakePointVisible(float x, float y, float topMarginRatio, float rightMarginRatio, float bottomMarginRatio, float leftMarginRatio, float pixelWidth) {
  // Range setters may stretch if range was too small for the new magnitude
  bool moved = false;
  if (std::isfinite(x)) {
    const float xRange = xMax() - xMin();
    const float leftMargin = leftMarginRatio * xRange;
    if (x < xMin() + leftMargin && xMin() > -k_maxFloat) {
      moved = true;
      /* The panning increment is a whole number of pixels so that the caching
       * for cartesian functions is not invalidated.
       * Cap to -k_maxFloat so that xRange is preserved. */
      const float newXMin = std::max(-k_maxFloat, std::floor((x - leftMargin - xMin()) / pixelWidth) * pixelWidth + xMin());
      protectedSetX(Range1D(newXMin, newXMin + xRange), k_maxFloat);
    }
    const float rightMargin = rightMarginRatio * xRange;
    if (x > xMax() - rightMargin && xMax() < k_maxFloat) {
      moved = true;
      const float newXMax = std::min(k_maxFloat, std::ceil((x + rightMargin - xMax()) / pixelWidth) * pixelWidth + xMax());
      protectedSetX(Range1D(newXMax - xRange, newXMax), k_maxFloat);
    }
  }
  if (std::isfinite(y)) {
    const float yRange = yMax() - yMin();
    const float bottomMargin = bottomMarginRatio * yRange;
    if (y < yMin() + bottomMargin && yMin() > -k_maxFloat) {
      moved = true;
      const float newYMin = std::max(-k_maxFloat, y - bottomMargin);
      protectedSetY(Range1D(newYMin, newYMin + yRange), k_maxFloat);
    }
    const float topMargin = topMarginRatio * yRange;
    if (y > yMax() - topMargin && yMax() < k_maxFloat) {
      moved = true;
      const float newYMax = std::min(k_maxFloat, y + topMargin);
      protectedSetY(Range1D(newYMax - yRange, newYMax), k_maxFloat);
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

bool InteractiveCurveViewRange::zoomOutToMakePointVisible(float x, float y, float topMarginRatio, float rightMarginRatio, float bottomMarginRatio, float leftMarginRatio) {
  Zoom zoom(-k_maxFloat, k_maxFloat, NormalYXRatio(), nullptr, k_maxFloat);
  zoom.fitPoint(Coordinate2D<float>(xMin(), yMin()));
  zoom.fitPoint(Coordinate2D<float>(xMax(), yMax()));
  zoom.fitPoint(Coordinate2D<float>(x, y), false, leftMarginRatio, rightMarginRatio, bottomMarginRatio, topMarginRatio);
  Range2D newRange = zoom.range(false, false);

  bool move = newRange != memoizedRange();
  protectedSetX(*newRange.x(), k_maxFloat);
  protectedSetY(*newRange.y(), k_maxFloat);

  if (move) {
    setZoomAuto(false);
    /* Zomming out to a point greater than the maximum range of 10^8 could make the
     * graph not normalized.*/
    setZoomNormalize(isOrthonormal());
  }

  return move;
}

void InteractiveCurveViewRange::protectedNormalize(bool canChangeX, bool canChangeY, bool canShrink) {
  /* We center the ranges on the current range center, and put each axis so that
   * 1cm = 2 current units. */

  if (isOrthonormal() || !(canChangeX || canChangeY)) {
    setZoomNormalize(isOrthonormal());
    return;
  }

  Range2D thisRange = memoizedRange();

  /* We try to normalize by expanding instead of shrinking as much as possible,
   * since shrinking can hide parts of the curve. If the axis we would like to
   * expand cannot be changed, we shrink the other axis instead, if allowed.
   * If the Y axis is too long, shrink Y if you cannot extend X ; but if the Y
   * axis is too short, shrink X if you cannot extend X. */
  bool shrink = thisRange.ratio() > NormalYXRatio() ? !canChangeX : !canChangeY;

  if (!shrink || canShrink) {
    thisRange.setRatio(NormalYXRatio(), shrink);

    protectedSetX(*thisRange.x(), k_maxFloat);
    protectedSetY(*thisRange.y(), k_maxFloat);

    /* The range should be close to orthonormal, unless :
     *   - it has been clipped because the maximum bounds have been reached.
     *   - the the bounds are too close and of too large a magnitude, leading to
     *     a drastic loss of significance. */
    assert(isOrthonormal()
        || xMin() <= - k_maxFloat || xMax() >= k_maxFloat || yMin() <= - k_maxFloat || yMax() >= k_maxFloat);
  }
  setZoomNormalize(isOrthonormal());
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

  if (m_delegate && (computeX || computeY)) {
    Range2D newRange;

    {
      CircuitBreakerCheckpoint checkpoint(Ion::CircuitBreaker::CheckpointType::Back);
      if (CircuitBreakerRun(checkpoint)) {
        uint32_t storeChecksum = Ion::Storage::FileSystem::sharedFileSystem()->checksum();
        if (computeX && computeY && m_storeChecksumOfLastComputedAutoRange == storeChecksum) {
          newRange = m_autoRange;
        } else {
          newRange = m_delegate->optimalRange(computeX, computeY, memoizedRange());
          if (computeX && computeY) {
            m_autoRange = newRange;
            m_storeChecksumOfLastComputedAutoRange = storeChecksum;
          }
        }
      } else {
        m_delegate->tidyModels();
        newRange = Zoom::DefaultRange(NormalYXRatio(), k_maxFloat);
      }
    }

    if (computeX) {
      protectedSetX(*newRange.x(), k_maxFloat);
    }
    if (computeY) {
      protectedSetY(*newRange.y(), k_maxFloat);
    }

    /* We notify the delegate to refresh the cursor's position, which will
     * update the bottom margin (which depends on the banner height). */
    m_delegate->refreshCursorAfterComputingRange();

    Range2D newRangeWithMargins = m_delegate->addMargins(newRange);
    newRangeWithMargins = Range2D(
        computeX ? *newRangeWithMargins.x() : Range1D(xMin(), xMax()),
        computeY ? *newRangeWithMargins.y() : Range1D(yMin(), yMax()));
    if (newRange.ratioIs(NormalYXRatio())) {
      newRangeWithMargins.setRatio(NormalYXRatio(), false);
      assert(newRangeWithMargins.ratioIs(NormalYXRatio()));
    }

    if (computeX) {
      protectedSetX(*newRangeWithMargins.x(), k_maxFloat);
    }
    if (computeY) {
      protectedSetY(*newRangeWithMargins.y(), k_maxFloat);
    }
  }

  setZoomNormalize(isOrthonormal());
}

}
