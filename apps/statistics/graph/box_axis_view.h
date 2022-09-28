#ifndef STATISTICS_BOX_AXIS_VIEW_H
#define STATISTICS_BOX_AXIS_VIEW_H

#include "box_range.h"
#include "../store.h"
#include <apps/constant.h>
#include <apps/shared/plot_view_policies.h>
#include <poincare/print_float.h>

namespace Statistics {

class BoxAxisView : public Shared::PlotView<Shared::PlotPolicy::LabeledXAxis, Shared::PlotPolicy::NoPlot, Shared::PlotPolicy::NoBanner, Shared::PlotPolicy::NoCursor> {
public:
  BoxAxisView(Store * store) : PlotView(&m_boxRange), m_boxRange(BoxRange(store)) {}

private:
  BoxRange m_boxRange;
};

}


#endif
