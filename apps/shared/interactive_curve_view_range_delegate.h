#ifndef SHARED_INTERACTIVE_CURVE_VIEW_DELEGATE_H
#define SHARED_INTERACTIVE_CURVE_VIEW_DELEGATE_H

#include <poincare/context.h>
#include <assert.h>
#include "function_store.h"

namespace Shared {

class InteractiveCurveViewRange;
class FunctionStore;

class InteractiveCurveViewRangeDelegate {
public:
  constexpr static float k_defaultXHalfRange = 10.0f;

  static void DefaultComputeXRange(float xMinLimit, float xMaxLimit, float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic, Poincare::Context * context, FunctionStore * functionStore);
  static void DefaultComputeYRange(float xMin, float xMax, float yMinIntrinsic, float yMaxIntrinsic, float ratio, float * yMin, float * yMax, Poincare::Context * context, FunctionStore * functionStore, bool optimizeRange);
  static void DefaultImproveFullRange(float * xMin, float * xMax, float * yMin, float * yMax, Poincare::Context * context, FunctionStore * functionStore);
  static float DefaultAddMargin(float x, float range, bool isVertical, bool isMin, float top, float bottom, float left, float right);

  virtual float interestingXMin() const { return -k_defaultXHalfRange; }
  virtual bool defaultRangeIsNormalized() const { return false; }
  virtual void computeXRange(float xMinLimit, float xMaxLimit, float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic) { assert(false); }
  virtual void computeYRange(float xMin, float xMax, float yMinIntrinsic, float yMaxIntrinsic, float * yMin, float * yMax, bool optimizeRange) { assert(false); }
  virtual void improveFullRange(float * xMin, float * xMax, float * yMin, float * yMax) { assert(false); }
  virtual float addMargin(float x, float range, bool isVertical, bool isMin) = 0;
  virtual bool canShrinkWhenNormalizing() const { return false; }
  virtual void updateBottomMargin() = 0;
  virtual void updateZoomButtons() = 0;
  virtual void tidyModels() = 0;
};

}

#endif
