#include "interactive_curve_view_range_delegate.h"
#include "interactive_curve_view_range.h"
#include "function_store.h"
#include <poincare/zoom.h>

namespace Shared {

void InteractiveCurveViewRangeDelegate::DefaultComputeXRange(float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic, Poincare::Context * context, FunctionStore * functionStore) {
  constexpr int maxLength = ExpressionModelStore::k_maxNumberOfMemoizedModels;
  float xMins[maxLength], xMaxs[maxLength], yMins[maxLength], yMaxs[maxLength];
  int length = functionStore->numberOfActiveFunctions();
  assert(length <= maxLength);

  for (int i = 0; i < length; i++) {
    ExpiringPointer<Function> f = functionStore->modelForRecord(functionStore->activeRecordAtIndex(i));
    f->xRangeForDisplay(xMins + i, xMaxs + i, yMins + i, yMaxs + i, context);
  }

  Poincare::Zoom::CombineRanges(length, xMins, xMaxs, xMin, xMax);
  Poincare::Zoom::CombineRanges(length, yMins, yMaxs, yMinIntrinsic, yMaxIntrinsic);
  Poincare::Zoom::SanitizeRange(xMin, xMax, yMinIntrinsic, yMaxIntrinsic, InteractiveCurveViewRange::NormalYXRatio());
}

void InteractiveCurveViewRangeDelegate::DefaultComputeYRange(float xMin, float xMax, float yMinIntrinsic, float yMaxIntrinsic, float ratio, float * yMin, float * yMax, Poincare::Context * context, FunctionStore * functionStore) {
  constexpr int maxLength = ExpressionModelStore::k_maxNumberOfMemoizedModels + 1;
  float yMins[maxLength], yMaxs[maxLength];
  int length = functionStore->numberOfActiveFunctions();
  assert(length <= maxLength);

  yMaxs[0] = yMaxIntrinsic;
  yMins[0] = yMinIntrinsic;
  for (int i = 0; i < length; i++) {
    ExpiringPointer<Function> f = functionStore->modelForRecord(functionStore->activeRecordAtIndex(i));
    f->yRangeForDisplay(xMin, xMax, yMins + 1 + i, yMaxs + 1 + i, context);
  }

  Poincare::Zoom::CombineRanges(length, yMins, yMaxs, yMin, yMax);
  Poincare::Zoom::SanitizeRange(&xMin, &xMax, yMin, yMax, InteractiveCurveViewRange::NormalYXRatio());
}

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

}
