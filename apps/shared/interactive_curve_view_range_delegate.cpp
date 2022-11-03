#include "interactive_curve_view_range_delegate.h"
#include "interactive_curve_view_range.h"
#include "function_store.h"
#include <poincare/zoom.h>

using namespace Poincare;

namespace Shared {

float InteractiveCurveViewRangeDelegate::DefaultAddMargin(float x, float range, bool isVertical, bool isMin, float top, float bottom, float left, float right) {
  /* The provided min or max range limit y is altered by adding a margin.
   * In pixels, the view's height occupied by the vertical range is equal to
   *   viewHeight - topMargin - bottomMargin.
   * Hence one pixel must correspond to
   *   range / (viewHeight - topMargin - bottomMargin).
   * Finally, adding topMargin pixels of margin, say at the top, comes down
   * to adding
   *   range * topMargin / (viewHeight - topMargin - bottomMargin)
   * which is equal to
   *   range * topMarginRatio / ( 1 - topMarginRatio - bottomMarginRatio)
   * where
   *   topMarginRation = topMargin / viewHeight
   *   bottomMarginRatio = bottomMargin / viewHeight.
   * The same goes horizontally.
   */
  float topMarginRatio = isVertical ? top : right;
  float bottomMarginRatio = isVertical ? bottom : left;
  assert(topMarginRatio + bottomMarginRatio < 1); // Assertion so that the formula is correct
  float ratioDenominator = 1 - bottomMarginRatio - topMarginRatio;
  float ratio = isMin ? -bottomMarginRatio : topMarginRatio;
  /* We want to add slightly more than the required margin, so that
   * InteractiveCurveViewRange::panToMakePointVisible does not think a point is
   * invisible due to precision problems when checking if it is outside the
   * required margin. This is why we add a 1.05f factor. */
  ratio = 1.05f * ratio / ratioDenominator;
  return x + ratio * range;
}

Range2D InteractiveCurveViewRangeDelegate::addMargins(Range2D range) const {
  float dx = range.x()->length();
  float xMin = addMargin(range.xMin(), dx, false, true);
  float xMax = addMargin(range.xMax(), dx, false, false);
  float dy = range.y()->length();
  float yMin = addMargin(range.yMin(), dy, true, true);
  float yMax = addMargin(range.yMax(), dy, true, false);
  return Range2D(xMin, xMax, yMin, yMax);
}

}
