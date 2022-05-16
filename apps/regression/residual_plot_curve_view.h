#ifndef REGRESSION_RESIDUAL_PLOT_CURVE_VIEW_H
#define REGRESSION_RESIDUAL_PLOT_CURVE_VIEW_H

#include <apps/shared/labeled_curve_view.h>
#include <apps/shared/cursor_view.h>
#include <apps/shared/banner_view.h>
#include <apps/shared/curve_view_cursor.h>
#include "store.h"

namespace Regression {

class ResidualPlotController;

class ResidualPlotCurveView : public Shared::LabeledCurveView {
public:
  ResidualPlotCurveView(
    Shared::CurveViewRange * curveViewRange,
    Shared::CurveViewCursor * curveViewCursor,
    Shared::BannerView * bannerView,
    Shared::CursorView * cursorView,
    ResidualPlotController * residualPlotController) :
      Shared::LabeledCurveView(curveViewRange, curveViewCursor, bannerView, cursorView),
      m_residualPlotController(residualPlotController) {}

  // Escher::View
  void drawRect(KDContext * ctx, KDRect rect) const override;

private:
  ResidualPlotController * m_residualPlotController;
};

}

#endif
