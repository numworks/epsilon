#ifndef STATISTICS_BOX_AXIS_VIEW_H
#define STATISTICS_BOX_AXIS_VIEW_H

#include "box_range.h"
#include "store.h"
#include "../shared/curve_view.h"
#include "../constant.h"

namespace Statistics {

class BoxAxisView : public Shared::CurveView {
public:
  BoxAxisView(Store * store) :
    CurveView(&m_boxRange),
    m_labels{},
    m_boxRange(BoxRange(store))
  {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_axisMargin = 3;
  char * label(Axis axis, int index) const override;
  char m_labels[k_maxNumberOfXLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  BoxRange m_boxRange;
};

}


#endif
