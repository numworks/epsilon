#ifndef STATISTICS_BOX_AXIS_VIEW_H
#define STATISTICS_BOX_AXIS_VIEW_H

#include <apps/constant.h>
#include <apps/shared/plot_view_policies.h>
#include <poincare/print_float.h>

#include "../store.h"
#include "box_range.h"

namespace Statistics {

class BoxAxisView
    : public Shared::PlotView<
          Shared::PlotPolicy::LabeledXAxis, Shared::PlotPolicy::NoPlot,
          Shared::PlotPolicy::NoBanner, Shared::PlotPolicy::NoCursor> {
 public:
  BoxAxisView(Store* store)
      : PlotView(&m_boxRange), m_boxRange(BoxRange(store)) {}

 private:
  BoxRange m_boxRange;
};

}  // namespace Statistics

#endif
