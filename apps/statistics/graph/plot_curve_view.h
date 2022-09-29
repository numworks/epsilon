#ifndef STATISTICS_PLOT_CURVE_VIEW_H
#define STATISTICS_PLOT_CURVE_VIEW_H

#include "../store.h"
#include <apps/shared/plot_view_policies.h>

namespace Statistics {

class PlotController;

class LabeledAxisWithOptionalPercent : public Shared::PlotPolicy::LabeledAxis {
public:
  void setPlotController(PlotController * controller) { m_plotController = controller; }

protected:
  float tickStep(const Shared::AbstractPlotView * plotView, Shared::AbstractPlotView::Axis axis) const;
  int computeLabel(int i, const Shared::AbstractPlotView * plotView, Shared::AbstractPlotView::Axis axis);

  PlotController * m_plotController;
};

typedef Shared::PlotPolicy::Axes<Shared::PlotPolicy::WithGrid, Shared::PlotPolicy::LabeledAxis, LabeledAxisWithOptionalPercent> PlotViewAxes;

class PlotViewPolicy {
protected:
  void drawPlot(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;
  void drawSeriesCurve(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect, int series) const;

  PlotController * m_plotController;
};

class PlotCurveView : public Shared::PlotView<PlotViewAxes, PlotViewPolicy, Shared::PlotPolicy::NoBanner, Shared::PlotPolicy::WithCursor> {
public:
  PlotCurveView(Shared::CurveViewRange * range, Shared::CurveViewCursor * cursor, Shared::CursorView * cursorView, PlotController * plotController);
};

}

#endif
