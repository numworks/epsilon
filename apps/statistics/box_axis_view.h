#ifndef STATISTICS_BOX_AXIS_VIEW_H
#define STATISTICS_BOX_AXIS_VIEW_H

#include "box_range.h"
#include "store.h"
#include "../shared/labeled_curve_view.h"
#include "../constant.h"
#include <poincare/print_float.h>

namespace Statistics {

class BoxAxisView : public Shared::HorizontallyLabeledCurveView {
public:
  BoxAxisView(Store * store) :
    HorizontallyLabeledCurveView(&m_boxRange),
    m_boxRange(BoxRange(store))
  {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_axisMargin = 3;
  BoxRange m_boxRange;
};

}


#endif
