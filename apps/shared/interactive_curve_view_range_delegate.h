#ifndef SHARED_INTERACTIVE_CURVE_VIEW_DELEGATE_H
#define SHARED_INTERACTIVE_CURVE_VIEW_DELEGATE_H

namespace Shared {

class InteractiveCurveViewRange;

class InteractiveCurveViewRangeDelegate {
public:
  bool didChangeRange(InteractiveCurveViewRange * interactiveCurveViewRange);
  virtual float interestingXRange();
protected:
  struct Range {
    float min;
    float max;
  };
private:
  virtual Range computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) = 0;
  virtual float addMargin(float x, float range, bool isMin) = 0;
};

}

#endif
