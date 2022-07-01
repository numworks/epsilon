#ifndef SEQUENCE_CURVE_VIEW_RANGE_H
#define SEQUENCE_CURVE_VIEW_RANGE_H

#include "../../shared/interactive_curve_view_range.h"

namespace Sequence {

class CurveViewRange : public Shared::InteractiveCurveViewRange {
public:
  CurveViewRange(Shared::InteractiveCurveViewRangeDelegate * delegate = nullptr);
private:
  bool hasDefaultRange() const override { return false; }
  void protectedNormalize(bool canChangeX, bool canChangeY, bool canShrink) override;
  constexpr static float k_displayLeftMarginRatio = 0.1f;
};

}

#endif
