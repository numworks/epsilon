#include "trigonometry_graph_cell.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Calculation {

void TrigonometryGraphPolicy::drawPlot(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  assert(m_model && plotView);

  float s = std::sin(m_model->angle());
  float c = std::cos(m_model->angle());

  // - Draw the unit circle
  Curve2D<float> circle = [](float t, void * model, void * context) {
    return Coordinate2D<float>(std::cos(t), std::sin(t));
  };
  CurveDrawing(circle, nullptr, nullptr, 0.f, 2.f * M_PI, M_PI / 180.f, Palette::GrayDark).draw(plotView, ctx, rect);

  // - Draw sine and cosine projections
  plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Horizontal, s, 0.f, c, Palette::Red, 1, 3);
  plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Vertical, c, 0.f, s, Palette::Red, 1, 3);

  // - Draw angle position on the circle
  plotView->drawDot(ctx, rect, Dots::Size::Large, Coordinate2D<float>(c, s), Palette::Red);

  // - Draw "sin(θ)" and "cos(θ)" labels
  plotView->drawLabel(ctx, rect, "sin(θ)", Coordinate2D<float>(0.f, s), c >= 0.f ? AbstractPlotView::RelativePosition::Before : AbstractPlotView::RelativePosition::After, AbstractPlotView::RelativePosition::There, Palette::Red);
  plotView->drawLabel(ctx, rect, "cos(θ)", Coordinate2D<float>(c, 0.f), AbstractPlotView::RelativePosition::There, s >= 0.f ? AbstractPlotView::RelativePosition::After : AbstractPlotView::RelativePosition::Before, Palette::Red);
}

TrigonometryGraphView::TrigonometryGraphView(TrigonometryModel * model) :
  PlotView(model)
{
  m_model = model;
}

}
