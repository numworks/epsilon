#ifndef SHARED_INTERACTIVE_CURVE_VIEW_DELEGATE_H
#define SHARED_INTERACTIVE_CURVE_VIEW_DELEGATE_H

#include <assert.h>

namespace Shared {

class InteractiveCurveViewRange;

class InteractiveCurveViewRangeDelegate {
public:
  bool didChangeRange(InteractiveCurveViewRange * interactiveCurveViewRange);
  virtual float interestingXMin() const { return -interestingXHalfRange(); }
  virtual float interestingXHalfRange() const { return 10.0f; }
  virtual bool defaultRangeIsNormalized() const { return false; }
  virtual void interestingRanges(InteractiveCurveViewRange * range) const { assert(false); }
  virtual float addMargin(float x, float range, bool isVertical, bool isMin) = 0;
protected:
  struct Range {
    float min;
    float max;
  };
private:
  virtual Range computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) = 0;
};

}

#endif
