#include "residual_plot_curve_view.h"
#include "residual_plot_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Regression {

// ResidualPlotPolicy

void ResidualPlotPolicy::drawPlot(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  KDColor color = m_controller->selectedSeriesColor();
  int numberOfDots = m_controller->numberOfResidualDots();
  for (int i = 0; i < numberOfDots; i++) {
    Coordinate2D<float> xy(m_controller->xAtIndex(i), m_controller->yAtIndex(i));
    plotView->drawDot(ctx, rect, Dots::Size::Tiny, xy, color);
  }
}

// ResidualPlotCurveView

ResidualPlotCurveView::ResidualPlotCurveView(CurveViewRange * range, CurveViewCursor * cursor, BannerView * banner, CursorView * cursorView, ResidualPlotController * controller) :
  PlotView(range)
{
  // ResidualPlotPolicy
  m_controller = controller;
  // WithBanner
  m_banner = banner;
  // WithCursor
  m_cursor = cursor;
  m_cursorView = cursorView;
}

}
