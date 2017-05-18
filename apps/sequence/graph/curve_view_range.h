#ifndef SEQUENCE_CURVE_VIEW_RANGE_H
#define SEQUENCE_CURVE_VIEW_RANGE_H

#include "../../shared/interactive_curve_view_range.h"

namespace Sequence {

class CurveViewRange : public Shared::InteractiveCurveViewRange {
public:
  CurveViewRange(Shared::CurveViewCursor * cursor, Shared::InteractiveCurveViewRangeDelegate * delegate = nullptr);
  void roundAbscissa() override;
  void normalize() override;
  void setTrigonometric() override;
  void setDefault() override;
private:
  constexpr static float k_displayLeftMarginRatio = 0.05f;
};

}

#endif
