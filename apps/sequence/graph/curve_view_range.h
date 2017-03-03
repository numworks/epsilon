#ifndef SEQUENCE_CURVE_VIEW_RANGE_H
#define SEQUENCE_CURVE_VIEW_RANGE_H

#include "../../shared/interactive_curve_view_range.h"

namespace Sequence {

class CurveViewRange : public Shared::InteractiveCurveViewRange {
public:
  CurveViewRange(Shared::CurveViewCursor * cursor, Shared::InteractiveCurveViewRangeDelegate * delegate);
  void setDefault() override;
private:
  constexpr static float k_displayLeftMarginRatio = 0.05f;
};

}

#endif
