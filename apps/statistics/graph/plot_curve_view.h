#ifndef STATISTICS_PLOT_CURVE_VIEW_H
#define STATISTICS_PLOT_CURVE_VIEW_H

#include "../store.h"
#include <apps/shared/plot_view_policies.h>

namespace Statistics {

class PlotController;

class PlotViewPolicy {
protected:
  void drawPlot(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;
  void drawSeriesCurve(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect, int series) const;

  PlotController * m_plotController;
};

// FIXME Use a custom Axes class to add % symbol to ordinate
class PlotCurveView : public Shared::PlotView<Shared::PlotPolicy::TwoLabeledAxes, PlotViewPolicy, Shared::PlotPolicy::NoBanner, Shared::PlotPolicy::WithCursor> {
public:
  PlotCurveView(Shared::CurveViewRange * range, Shared::CurveViewCursor * cursor, Shared::CursorView * cursorView, PlotController * plotController);
};

}

#endif
