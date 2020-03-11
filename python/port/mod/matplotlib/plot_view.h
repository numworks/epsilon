#ifndef PYTHON_MATPLOTLIB_PLOT_VIEW_H
#define PYTHON_MATPLOTLIB_PLOT_VIEW_H

#include <apps/shared/labeled_curve_view.h>
#include "plot_store.h"

namespace Matplotlib {

class PlotView : public Shared::LabeledCurveView {
public:
  PlotView(PlotStore * s) : Shared::LabeledCurveView(s) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

}


#endif
