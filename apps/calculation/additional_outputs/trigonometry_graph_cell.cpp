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
  Curve2D circle([](float t, void * model, void * context) {
    return Coordinate2D<float>(std::cos(t), std::sin(t));
  });
  CurveDrawing(circle, nullptr, 0.f, 2.f * M_PI, M_PI / 180.f, KDColorBlack, false).draw(plotView, ctx, rect);

  // - Draw sine and cosine projections
  plotView->drawDashedStraightSegment(ctx, rect, AbstractPlotView::Axis::Horizontal, s, 0.f, c, Palette::Red);
  plotView->drawDashedStraightSegment(ctx, rect, AbstractPlotView::Axis::Vertical, c, 0.f, s, Palette::Red);
  plotView->drawSegment(ctx, rect, {0.f, 0.f}, {c, s}, Palette::Red);

  // - Draw angle position on the circle
  plotView->drawDot(ctx, rect, Dots::Size::Large, Coordinate2D<float>(c, s), Palette::Red);

  // - Draw angle arc and label
  float angle = m_model->angle();
  constexpr float k_arcRatio = 0.2;
  constexpr float k_labelRatio = 0.32;
  bool labelOnLine = std::abs(angle) < M_PI/30.f || std::abs(M_PI - angle) < M_PI/30.f;
  plotView->drawLabel(ctx, rect, "θ", {k_labelRatio * std::cos(angle/2), k_labelRatio * std::sin(angle/2)}, AbstractPlotView::RelativePosition::There, labelOnLine ? AbstractPlotView::RelativePosition::Before : AbstractPlotView::RelativePosition::There, Palette::Red);
  // Draw the arc
  if (angle < 0) {
    drawArcOfEllipse(plotView, ctx, rect, {0.f, 0.f}, k_arcRatio, k_arcRatio, angle, 0.0f, Palette::Red);
  } else {
    drawArcOfEllipse(plotView, ctx, rect, {0.f, 0.f}, k_arcRatio, k_arcRatio, 0.0f, angle, Palette::Red);
  }

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
