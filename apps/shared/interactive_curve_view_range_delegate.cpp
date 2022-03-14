#include "interactive_curve_view_range_delegate.h"
#include "interactive_curve_view_range.h"
#include "function_store.h"
#include <poincare/zoom.h>

namespace Shared {

void InteractiveCurveViewRangeDelegate::DefaultComputeXRange(float xMinLimit, float xMaxLimit, float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic, Poincare::Context * context, FunctionStore * functionStore) {
  float xMinTemp, xMaxTemp, yMinTemp, yMaxTemp;
  *xMin = FLT_MAX;
  *xMax = -FLT_MAX;
  *yMinIntrinsic = FLT_MAX;
  *yMaxIntrinsic = -FLT_MAX;
  int length = functionStore->numberOfActiveFunctions();

  for (int i = 0; i < length; i++) {
    ExpiringPointer<Function> f = functionStore->modelForRecord(functionStore->activeRecordAtIndex(i));
    // Reinitialize temporary variables in case xRangeForDisplay doesn't.
    xMinTemp = xMaxTemp = yMinTemp = yMaxTemp = NAN;
    f->xRangeForDisplay(xMinLimit, xMaxLimit, &xMinTemp, &xMaxTemp, &yMinTemp, &yMaxTemp, context);
    Poincare::Zoom::CombineRanges(xMinTemp, xMaxTemp, *xMin, *xMax, xMin, xMax);
    Poincare::Zoom::CombineRanges(yMinTemp, yMaxTemp, *yMinIntrinsic, *yMaxIntrinsic, yMinIntrinsic, yMaxIntrinsic);
  }

  Poincare::Zoom::SanitizeRangeForDisplay(xMin, xMax);
}

void InteractiveCurveViewRangeDelegate::DefaultComputeYRange(float xMin, float xMax, float yMinIntrinsic, float yMaxIntrinsic, float ratio, float * yMin, float * yMax, Poincare::Context * context, FunctionStore * functionStore, bool optimizeRange) {
  int length = functionStore->numberOfActiveFunctions();

  float yMinTemp, yMaxTemp;

  *yMin = yMinIntrinsic;
  *yMax = yMaxIntrinsic;
  for (int i = 0; i < length; i++) {
    ExpiringPointer<Function> f = functionStore->modelForRecord(functionStore->activeRecordAtIndex(i));
    f->yRangeForDisplay(xMin, xMax, yMinIntrinsic, yMaxIntrinsic, ratio, &yMinTemp, &yMaxTemp, context, optimizeRange);
    Poincare::Zoom::CombineRanges(yMinTemp, yMaxTemp, *yMin, *yMax, yMin, yMax);
  }

  Poincare::Zoom::SanitizeRangeForDisplay(yMin, yMax, ratio * (xMax - xMin) / 2.f);
}

void InteractiveCurveViewRangeDelegate::DefaultImproveFullRange(float * xMin, float * xMax, float * yMin, float * yMax, Poincare::Context * context, FunctionStore * functionStore) {
  if (functionStore->numberOfActiveFunctions() == 1) {
    ExpiringPointer<Function> f = functionStore->modelForRecord(functionStore->activeRecordAtIndex(0));
    if (!f->basedOnCostlyAlgorithms(context)) {
      Poincare::Zoom::ValueAtAbscissa evaluation = [](float x, Poincare::Context * context, const void * auxiliary) {
        return static_cast<const Function *>(auxiliary)->evaluateXYAtParameter(x, context).x2();
      };
      Poincare::Zoom::ExpandSparseWindow(evaluation, xMin, xMax, yMin, yMax, context, f.operator->());
    }
  }
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
