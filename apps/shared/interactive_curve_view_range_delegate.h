#ifndef SHARED_INTERACTIVE_CURVE_VIEW_DELEGATE_H
#define SHARED_INTERACTIVE_CURVE_VIEW_DELEGATE_H

#include <poincare/context.h>
#include <assert.h>

namespace Shared {

class InteractiveCurveViewRange;
class FunctionStore;

class InteractiveCurveViewRangeDelegate {
public:
  static constexpr float k_defaultXHalfRange = 10.0f;

  static void InterestingRangesHelper(InteractiveCurveViewRange * range, Poincare::Context * context, FunctionStore * functionStore, float targetRatio);
  static float AddMarginHelper(float x, float range, bool isVertical, bool isMin, float top, float bottom, float left, float right);

  virtual float interestingXMin() const { return -k_defaultXHalfRange; }
  virtual bool defaultRangeIsNormalized() const { return false; }
  virtual void interestingRanges(InteractiveCurveViewRange * range) { assert(false); }
  virtual float addMargin(float x, float range, bool isVertical, bool isMin) = 0;
  virtual void updateZoomButtons() = 0;
};

}

#endif
