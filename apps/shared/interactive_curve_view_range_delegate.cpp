#include "interactive_curve_view_range_delegate.h"
#include "interactive_curve_view_range.h"
#include <cmath>
#include <float.h>

namespace Shared {

bool InteractiveCurveViewRangeDelegate::didChangeRange(InteractiveCurveViewRange * interactiveCurveViewRange) {
  /* When y auto is ticked, top and bottom margins are added to ensure that
   * the cursor can be move along the curve, in the current x-range,
   * without panning the window. */
  if (!interactiveCurveViewRange->yAuto()) {
    return false;
  }
  Range yRange = computeYRange(interactiveCurveViewRange);
  float max = yRange.max;
  float min = yRange.min;
  float range = max - min;
  if (max < min) {
    range = 0.0f;
  }
  if (interactiveCurveViewRange->yMin() == addMargin(min, range, true) && interactiveCurveViewRange->yMax() == addMargin(max, range, false)) {
    return false;
  }
  if (min == FLT_MAX && max == -FLT_MAX) {
    min = -1.0f;
    max = 1.0f;
  }
  range = max - min;
  interactiveCurveViewRange->setYMin(addMargin(min, range, true));
  interactiveCurveViewRange->setYMax(addMargin(max, range, false));
  if (std::isinf(interactiveCurveViewRange->xMin())) {
    interactiveCurveViewRange->setYMin(-FLT_MAX);
  }
  if (std::isinf(interactiveCurveViewRange->xMax())) {
    interactiveCurveViewRange->setYMax(FLT_MAX);
  }
  return true;
}

}
