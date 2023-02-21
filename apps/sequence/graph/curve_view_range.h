#ifndef SEQUENCE_CURVE_VIEW_RANGE_H
#define SEQUENCE_CURVE_VIEW_RANGE_H

#include "../../shared/interactive_curve_view_range.h"

namespace Sequence {

class GraphController;

class CurveViewRange : public Shared::InteractiveCurveViewRange {
 public:
  CurveViewRange(GraphController* delegate = nullptr);

 private:
  constexpr static float k_displayLeftMarginRatio = 0.1f;

  bool hasDefaultRange() const override { return false; }
  void protectedNormalize(bool canChangeX, bool canChangeY,
                          bool canShrink) override;
};

}  // namespace Sequence

#endif
