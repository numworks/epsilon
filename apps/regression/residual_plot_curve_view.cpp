#include "residual_plot_curve_view.h"
#include "residual_plot_controller.h"

namespace Regression {

void ResidualPlotCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  simpleDrawBothAxesLabels(ctx, rect);
  KDColor color = m_residualPlotController->selectedSeriesColor();
  int numberOfDots = m_residualPlotController->numberOfResidualDots();
  for (int i = 0; i < numberOfDots; i++) {
    double x = m_residualPlotController->xAtIndex(i);
    double y = m_residualPlotController->yAtIndex(i);
    Shared::CurveView::drawDot(ctx, rect, x, y, color);
  }
  Shared::LabeledCurveView::drawRect(ctx, rect);
}

}
