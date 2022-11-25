#ifndef REGRESSION_RESIDUAL_PLOT_CURVE_VIEW_H
#define REGRESSION_RESIDUAL_PLOT_CURVE_VIEW_H

#include "../store.h"
#include <apps/shared/plot_view_policies.h>

namespace Regression {

class ResidualPlotController;

class ResidualPlotPolicy {
protected:
  void drawPlot(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;

  ResidualPlotController * m_controller;
};

class ResidualPlotCurveView : public Shared::PlotView<Shared::PlotPolicy::TwoLabeledAxes, ResidualPlotPolicy, Shared::PlotPolicy::WithBanner, Shared::PlotPolicy::WithCursor> {
public:
  ResidualPlotCurveView(Shared::CurveViewRange * range, Shared::CurveViewCursor * cursor, Shared::BannerView * banner, Shared::CursorView * cursorView, ResidualPlotController * controller);
};

}

#endif
