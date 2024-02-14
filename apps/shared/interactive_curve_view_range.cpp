#include "interactive_curve_view_range.h"

#include <assert.h>
#include <ion.h>
#include <omg/ieee754.h>
#include <omg/numeric_comparison.h>
#include <poincare/numeric/zoom.h>
#include <poincare/old/circuit_breaker_checkpoint.h>
#include <poincare/preferences.h>
#include <stddef.h>

#include <algorithm>
#include <cmath>

using namespace Poincare;

namespace Shared {

bool InteractiveCurveViewRange::isOrthonormal() const {
  Range2D<float> range = memoizedRange();
  range = Range2D<float>(
      *range.x(),
      Range1D<float>(range.yMin(), range.yMax() + offscreenYAxis()));
  return range.ratioIs(NormalYXRatio());
}

void InteractiveCurveViewRange::setDelegate(
    InteractiveCurveViewRangeDelegate* delegate) {
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
  float magnitude =
      std::pow(10.0f, OMG::IEEE754<float>::exponentBase10(range) - 1.0f);
  if (isMin) {
    return magnitude * std::floor(y / magnitude);
  }
  return magnitude * std::ceil(y / magnitude);
}

void InteractiveCurveViewRange::setXRange(float min, float max) {
  assert(!m_zoomAuto.x || m_delegate == nullptr);
  MemoizedCurveViewRange::protectedSetXRange(min, max, k_maxFloat);
  computeRanges();
}

void InteractiveCurveViewRange::setYRange(float min, float max) {
  assert(!m_zoomAuto.y || m_delegate == nullptr);
  MemoizedCurveViewRange::protectedSetYRange(min, max, k_maxFloat);
  setZoomNormalize(isOrthonormal());
}

void InteractiveCurveViewRange::setOffscreenYAxis(float f) {
  if (f == m_offscreenYAxis) {
    return;
  }
  float d = m_offscreenYAxis - f;
  m_offscreenYAxis = f;
  MemoizedCurveViewRange::protectedSetYRange(yMin(), yMax() + d, k_maxFloat);
}

float InteractiveCurveViewRange::xGridUnit() const {
  if (!gridUnitAuto(Axis::X)) {
    return computeGridUnitFromUserParameter(Axis::X);
  }
  if (m_zoomNormalize) {
    float yUnit = yGridUnit();
    if ((xMax() - xMin()) / yUnit <= k_maxNumberOfXGridUnits) {
      return yUnit;
    }
  }
  return MemoizedCurveViewRange::xGridUnit();
}

float InteractiveCurveViewRange::yGridUnit() const {
  if (!gridUnitAuto(Axis::Y)) {
    return computeGridUnitFromUserParameter(Axis::Y);
  }
  float res = MemoizedCurveViewRange::yGridUnit();
  if (m_zoomNormalize) {
    /* When m_zoomNormalize is active, both xGridUnit and yGridUnit will be the
     * same. To declutter the X axis, we try a unit twice as large. We check
     * that it allows enough graduations on the Y axis, but if the standard
     * unit would lead to too many graduations on the X axis, we force the
     * larger unit anyways. */
    float numberOfYUnits = (yMax() - yMin() + offscreenYAxis()) / res;
    float numberOfXUnits = (xMax() - xMin()) / res;
    if (numberOfXUnits > k_maxNumberOfXGridUnits ||
        numberOfYUnits / 2.f > k_minNumberOfYGridUnits) {
      return 2 * res;
    }
  }
  return res;
}

void InteractiveCurveViewRange::zoom(float ratio, float x, float y) {
  Range2D<float> thisRange = memoizedRange();
  float dy = thisRange.y()->length();
  if (ratio * std::min(thisRange.x()->length(), dy) <
      Range1D<float>::k_minLength) {
    return;
  }
  Coordinate2D<float> center(std::isfinite(x) ? x : thisRange.x()->center(),
                             std::isfinite(y) ? y : thisRange.y()->center());
  thisRange.zoom(ratio, center);
  assert(!thisRange.x()->isNan() && !thisRange.y()->isNan());
  setZoomAuto(false);
  protectedSetXRange(*thisRange.x(), k_maxFloat);
  protectedSetYRange(*thisRange.y(), k_maxFloat);
  /* The factor will typically be equal to ratio, unless yMax and yMin are
   * close to the maximal values. */
  float yRatio = memoizedRange().y()->length() / dy;
  m_offscreenYAxis *= yRatio;
  setZoomNormalize(isOrthonormal());
}

static float vectorLengthForMove(float v, float x) {
  float minLength = std::nextafter(x, std::copysign(INFINITY, v)) - x;
  assert((minLength < 0.f) == (v < 0.f));
  return OMG::WithGreatestAbs(v, minLength);
}

void InteractiveCurveViewRange::panWithVector(float x, float y) {
  Range1D<float> xRange(xMin(), xMax(), k_maxFloat);
  if (x != 0.f) {
    x = OMG::WithGreatestAbs(vectorLengthForMove(x, xMin()),
                             vectorLengthForMove(x, xMax()));
    xRange =
        Range1D<float>::ValidRangeBetween(xMin() + x, xMax() + x, k_maxFloat);
    if (xRange.min() != xMin() + x || xRange.max() != xMax() + x) {
      return;
    }
  }

  Range1D<float> yRange(yMin(), yMax(), k_maxFloat);
  if (y != 0.f) {
    y = OMG::WithGreatestAbs(vectorLengthForMove(y, yMin()),
                             vectorLengthForMove(y, yMax()));
    yRange =
        Range1D<float>::ValidRangeBetween(yMin() + y, yMax() + y, k_maxFloat);
    if (yRange.min() != yMin() + y || yRange.max() != yMax() + y) {
      return;
    }
  }

  if (x != 0.f || y != 0.f) {
    setZoomAuto(false);
    protectedSetXRange(xRange, k_maxFloat);
    protectedSetYRange(yRange, k_maxFloat);
  }
}

void InteractiveCurveViewRange::normalize() {
  /* If one axis is set manually and the other is in auto mode, prioritize
   * changing the auto one. */
  bool canChangeX = m_zoomAuto.x || !m_zoomAuto.y;
  bool canChangeY = m_zoomAuto.y || !m_zoomAuto.x;
  setZoomAuto(false);
  protectedNormalize(canChangeX, canChangeY, !canChangeX || !canChangeY);
}

void InteractiveCurveViewRange::centerAxisAround(Axis axis, float position) {
  if (std::isnan(position)) {
    return;
  }
  if (axis == Axis::X) {
    float range = xMax() - xMin();
    if (std::fabs(position / range) > k_maxRatioPositionRange) {
      range = Range1D<float>::DefaultLengthAt(position);
    }
    float newXMax = position + range / 2.0f;
    if (xMax() != newXMax) {
      setZoomAuto(false);
      protectedSetXRange(newXMax - range, newXMax, k_maxFloat);
    }
  } else {
    float range = yMax() - yMin();
    if (std::fabs(position / range) > k_maxRatioPositionRange) {
      range = Range1D<float>::DefaultLengthAt(position);
    }
    float newYMax = position + range / 2.0f;
    if (yMax() != newYMax) {
      setZoomAuto(false);
      protectedSetYRange(position - 0.5f * range, position + 0.5f * range,
                         k_maxFloat);
    }
  }

  setZoomNormalize(isOrthonormal());
}

bool InteractiveCurveViewRange::panToMakePointVisible(
    float x, float y, float topMarginRatio, float rightMarginRatio,
    float bottomMarginRatio, float leftMarginRatio, float pixelWidth) {
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
      const float newXMin = std::max(
          -k_maxFloat,
          std::floor((x - leftMargin - xMin()) / pixelWidth) * pixelWidth +
              xMin());
      protectedSetXRange(newXMin, newXMin + xRange, k_maxFloat);
    }
    const float rightMargin = rightMarginRatio * xRange;
    if (x > xMax() - rightMargin && xMax() < k_maxFloat) {
      moved = true;
      const float newXMax = std::min(
          k_maxFloat,
          std::ceil((x + rightMargin - xMax()) / pixelWidth) * pixelWidth +
              xMax());
      protectedSetXRange(newXMax - xRange, newXMax, k_maxFloat);
    }
  }
  if (std::isfinite(y)) {
    const float yRange = yMax() - yMin();
    const float bottomMargin = bottomMarginRatio * yRange;
    if (y < yMin() + bottomMargin && yMin() > -k_maxFloat) {
      moved = true;
      const float newYMin = std::max(-k_maxFloat, y - bottomMargin);
      protectedSetYRange(newYMin, newYMin + yRange, k_maxFloat);
    }
    const float topMargin = topMarginRatio * yRange;
    if (y > yMax() - topMargin && yMax() < k_maxFloat) {
      moved = true;
      const float newYMax = std::min(k_maxFloat, y + topMargin);
      protectedSetYRange(newYMax - yRange, newYMax, k_maxFloat);
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

bool InteractiveCurveViewRange::zoomOutToMakePointVisible(
    float x, float y, float topMarginRatio, float rightMarginRatio,
    float bottomMarginRatio, float leftMarginRatio) {
  Zoom zoom(-k_maxFloat, k_maxFloat, NormalYXRatio(), k_maxFloat);
  zoom.fitPoint(Coordinate2D<float>(xMin(), yMin()));
  zoom.fitPoint(Coordinate2D<float>(xMax(), yMax()));
  zoom.fitPoint(Coordinate2D<float>(x, y), false, leftMarginRatio,
                rightMarginRatio, bottomMarginRatio, topMarginRatio);
  Range2D<float> newRange = zoom.range(false, false);

  bool move = newRange != memoizedRange();
  protectedSetXRange(*newRange.x(), k_maxFloat);
  protectedSetYRange(*newRange.y(), k_maxFloat);

  if (move) {
    setZoomAuto(false);
    /* Zomming out to a point greater than the maximum range of 10^8 could make
     * the graph not normalized.*/
    setZoomNormalize(isOrthonormal());
  }

  return move;
}

void InteractiveCurveViewRange::setGridType(GridType grid) {
  m_gridType = grid;

  /* Reset memoized auto range as it may depend on the grid type: polar grid
   * needs normalization, which is delegated to m_delegate::optimalRange.*/
  m_checksumOfMemoizedAutoRange = 0;
}

void InteractiveCurveViewRange::protectedNormalize(bool canChangeX,
                                                   bool canChangeY,
                                                   bool canShrink) {
  /* We center the ranges on the current range center, and put each axis so that
   * 1cm = 2 current units. */

  if (isOrthonormal() || !(canChangeX || canChangeY)) {
    setZoomNormalize(isOrthonormal());
    return;
  }

  Range2D<float> thisRange = memoizedRange();

  /* We try to normalize by expanding instead of shrinking as much as possible,
   * since shrinking can hide parts of the curve. If the axis we would like to
   * expand cannot be changed, we shrink the other axis instead, if allowed.
   * If the Y axis is too long, shrink Y if you cannot extend X ; but if the Y
   * axis is too short, shrink X if you cannot extend X. */
  bool shrink = thisRange.ratio() > NormalYXRatio() ? !canChangeX : !canChangeY;

  if (!shrink || canShrink) {
    bool canSetRatio = thisRange.setRatio(NormalYXRatio(), shrink, k_maxFloat);
    if (canSetRatio) {
      protectedSetXRange(*thisRange.x(), k_maxFloat);
      protectedSetYRange(*thisRange.y(), k_maxFloat);
      assert(isOrthonormal());
    }
  }
  setZoomNormalize(isOrthonormal());
}

void InteractiveCurveViewRange::privateSetZoomAuto(bool xAuto, bool yAuto) {
  bool oldAuto = zoomAndGridUnitAuto();
  m_zoomAuto.x = xAuto;
  m_zoomAuto.y = yAuto;
  if (m_delegate && oldAuto != zoomAndGridUnitAuto()) {
    m_delegate->updateZoomButtons();
  }
}

void InteractiveCurveViewRange::privateSetUserGridUnit(float xValue,
                                                       float yValue) {
  bool oldAuto = zoomAndGridUnitAuto();
  m_userGridUnit.x = xValue;
  m_userGridUnit.y = yValue;
  if (m_delegate && oldAuto != zoomAndGridUnitAuto()) {
    m_delegate->updateZoomButtons();
  }
}

void InteractiveCurveViewRange::privateComputeRanges(bool computeX,
                                                     bool computeY) {
  assert(offscreenYAxis() == 0.f);

  /* If m_zoomNormalize was left active, xGridUnit() would return the value of
   * yGridUnit, even if the range were not truly normalized. We use
   * setZoomNormalize to refresh the button in case the graph does not end up
   * normalized. */
  setZoomNormalize(false);
  if (m_delegate && (computeX || computeY)) {
    Range2D<float> newRange;
    bool useMemoizedAutoRange = computeX && computeY;
    {
      CircuitBreakerCheckpoint checkpoint(
          Ion::CircuitBreaker::CheckpointType::Back);
      if (CircuitBreakerRun(checkpoint)) {
        uint64_t checksum;
        if (useMemoizedAutoRange &&
            (checksum = m_delegate->autoZoomChecksum()) ==
                m_checksumOfMemoizedAutoRange) {
          newRange = m_memoizedAutoRange;
        } else {
          newRange =
              m_delegate->optimalRange(computeX, computeY, memoizedRange());
          if (useMemoizedAutoRange) {
            m_memoizedAutoRange = newRange;
            m_checksumOfMemoizedAutoRange = checksum;
          }
        }
      } else {
        m_delegate->tidyModels(checkpoint.endOfPoolBeforeCheckpoint());
        newRange = Zoom::DefaultRange(NormalYXRatio(), k_maxFloat);
      }
    }

    if (computeX) {
      protectedSetXRange(*newRange.x(), k_maxFloat);
    }
    if (computeY) {
      protectedSetYRange(*newRange.y(), k_maxFloat);
    }

    /* We notify the delegate to refresh the cursor's position, which will
     * update the bottom margin (which depends on the banner height). */
    m_delegate->refreshCursorAfterComputingRange();

    Range2D<float> newRangeWithMargins = m_delegate->addMargins(newRange);
    newRangeWithMargins =
        Range2D<float>(computeX ? *newRangeWithMargins.x()
                                : Range1D<float>(xMin(), xMax(), k_maxFloat),
                       computeY ? *newRangeWithMargins.y()
                                : Range1D<float>(yMin(), yMax(), k_maxFloat));
    if (newRange.ratioIs(NormalYXRatio())) {
      bool canSetRatio =
          newRangeWithMargins.setRatio(NormalYXRatio(), false, k_maxFloat);
      assert(!canSetRatio || newRangeWithMargins.ratioIs(NormalYXRatio()));
      (void)canSetRatio;
    }

    if (computeX) {
      protectedSetXRange(*newRangeWithMargins.x(), k_maxFloat);
    }
    if (computeY) {
      protectedSetYRange(*newRangeWithMargins.y(), k_maxFloat);
    }
  }

  setZoomNormalize(isOrthonormal());
}

float InteractiveCurveViewRange::computeGridUnitFromUserParameter(
    Axis axis) const {
  assert(!gridUnitAuto(axis));
  float minNumberOfUnits, maxNumberOfUnits, range;
  if (axis == Axis::X) {
    minNumberOfUnits = k_minNumberOfXGridUnits;
    maxNumberOfUnits = k_maxNumberOfXGridUnits;
    range = xMax() - xMin();
  } else {
    assert(axis == Axis::Y);
    minNumberOfUnits = k_minNumberOfYGridUnits;
    maxNumberOfUnits = k_maxNumberOfYGridUnits;
    range = yMax() - yMin() + offscreenYAxis();
  }
  assert(range > 0.0f && std::isfinite(range));
  float userGridUnit = m_userGridUnit(axis);
  assert(userGridUnit > 0.0f);
  float numberOfUnits = range / userGridUnit;  // in float for now
  if (minNumberOfUnits <= numberOfUnits && numberOfUnits <= maxNumberOfUnits) {
    // Case 1
    return userGridUnit;
  } else if (numberOfUnits < minNumberOfUnits) {
    // Case 2
    assert(std::ceil(minNumberOfUnits / numberOfUnits) <=
           std::floor(maxNumberOfUnits / numberOfUnits));
    int k = std::ceil(minNumberOfUnits / numberOfUnits);
    return userGridUnit / k;
  }
  assert(numberOfUnits > maxNumberOfUnits);
  // Case 3
  assert(std::ceil(numberOfUnits / maxNumberOfUnits) <=
         std::floor(numberOfUnits / minNumberOfUnits));
  int k = std::ceil(numberOfUnits / maxNumberOfUnits);
  return userGridUnit * k;

  // clang-format off
  /* Proof of the algorithm:
   *
   * We want to find gridUnit = userGridUnit * k or gridUnit = userGridUnit / k, with k an integer.
   * We want: minNumberOfUnits <= range / gridUnit <= maxNumberOfUnits
   *
   * Case 1: minNumberOfUnits <= range / userGridUnit <= maxNumberOfUnits
   * ------
   * The solution is userGridUnit.
   *
   * Case 2: range / userGridUnit < minNumberOfUnits
   * -------
   * We want to decrease the grid unit, so we look for gridUnit = userGridUnit / k, with k an integer
   * A solution thus needs to verify:
   *       minNumberOfUnits <= range / (userGridUnit / k) <= maxNumberOfUnits
   * <=>   minNumberOfUnits <=  k * range / userGridUnit  <= maxNumberOfUnits
   * <=>   E1 = minNumberOfUnits * userGridUnit / range <= k <= maxNumberOfUnits * userGridUnit / range = E2
   * Since k must be a integer,
   * <=>   ceil(E1) <= k <= floor(E2)
   *
   * We have a solution if ceil(E1) <= floor(E2) <=> floor(E1) < floor(E2) <=> floor(E1) != floor(E2)
   *
   * Let's compute E2 - E1:
   * E2 - E1 = (maxNumberOfUnits - minNumberOfUnits) * userGridUnit / range
   * and since range / userGridUnit < minNumberOfUnits
   * E2 - E1 > (maxNumberOfUnits - minNumberOfUnits) / minNumberOfUnits = E3
   * For minNumberOfUnits = 7 and maxNumberOfUnits = 18, E3 = 1.57...
   * For minNumberOfUnits = 5 and maxNumberOfUnits = 13, E3 = 1.6
   * => E2 - E1 > 1.5
   * => floor(E1) != floor(E2)
   *
   * We can take the smallest k solution to be as close as possible to the user input:
   * ceil(E1) = ceil(minNumberOfUnits * userGridUnit / range)
   *
   * Case 3: range / userGridUnit > maxNumberOfUnits
   * -------
   * We want to increase the grid unit, so we look for gridUnit = userGridUnit * k, with k an integer
   * Similar computation than in case 2 will give
   * E1 = range / (maxNumberOfUnits * userGridUnit)
   * E2 = range / (minNumberOfUnits * userGridUnit)
   * Let's compute E2 - E1:
   * E2 - E1 = (1/minNumberOfUnits - 1/maxNumberOfUnits) * range / userGridUnit
   * and since range / userGridUnit > maxNumberOfUnits
   * E2 - E1 > (1/minNumberOfUnits - 1/maxNumberOfUnits) * maxNumberOfUnits = (maxNumberOfUnits - minNumberOfUnits) / minNumberOfUnits = E3
   * We saw in case 2 that E3 > 1.5
   * => E2 - E1 > 1.5
   * => floor(E1) != floor(E2)
   *
   * We can take the smallest k solution to be as close as possible to the user input:
   * ceil(E1) = ceil(range / (maxNumberOfUnits * userGridUnit))
   * */
  // clang-format on
}

}  // namespace Shared
