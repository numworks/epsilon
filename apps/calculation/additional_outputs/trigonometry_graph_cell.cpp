#include "trigonometry_graph_cell.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Calculation {

void TrigonometryGraphPolicy::drawPlot(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  assert(m_model && plotView);

  float s = std::sin(m_model->angle());
  float c = std::cos(m_model->angle());

  // - Draw sine and cosine projections
  plotView->drawDashedStraightSegment(ctx, rect, AbstractPlotView::Axis::Horizontal, s, 0.f, c, Palette::Red);
  plotView->drawDashedStraightSegment(ctx, rect, AbstractPlotView::Axis::Vertical, c, 0.f, s, Palette::Red);
  plotView->drawSegment(ctx, rect, {0.f, 0.f}, {c, s}, Palette::Red);

  // - Draw angle position on the circle
  plotView->drawDot(ctx, rect, Dots::Size::Large, Coordinate2D<float>(c, s), Palette::Red);

  // - Draw angle arc and label
  float angle = m_model->angle();
  assert(0 <= angle && angle < 2 * M_PI + Float<float>::EpsilonLax());
  constexpr float k_arcRatio = 0.2;
  constexpr float k_labelRatio = 0.32;
  bool labelOnLine = std::fabs(angle) < M_PI / 30.f || std::fabs(M_PI - angle) < M_PI / 30.f;
  plotView->drawLabel(ctx, rect, "θ", {k_labelRatio * std::cos(angle / 2), k_labelRatio * std::sin(angle/2)}, AbstractPlotView::RelativePosition::There, labelOnLine ? AbstractPlotView::RelativePosition::Before : AbstractPlotView::RelativePosition::There, Palette::Red);
  // Draw the arc
  drawArcOfEllipse(plotView, ctx, rect, {0.f, 0.f}, k_arcRatio, k_arcRatio, 0.0f, angle, Palette::Red);

  // - Draw "sin(θ)" and "cos(θ)" labels
  plotView->drawLabel(ctx, rect, "sin(θ)", Coordinate2D<float>(0.f, s), c >= 0.f ? AbstractPlotView::RelativePosition::Before : AbstractPlotView::RelativePosition::After, AbstractPlotView::RelativePosition::There, Palette::Red);
  plotView->drawLabel(ctx, rect, "cos(θ)", Coordinate2D<float>(c, 0.f), AbstractPlotView::RelativePosition::There, s >= 0.f ? AbstractPlotView::RelativePosition::After : AbstractPlotView::RelativePosition::Before, Palette::Red);
}

void UnitCircle::drawAxesAndGrid(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  PlotPolicy::WithPolarGrid::drawGrid(plotView, ctx, rect);
  plotView->drawCircle(ctx, rect, {0.f, 0.f}, 1.f, KDColorBlack);
  // Labels should be as close to the axes as possible so we place them manually
  KDRect labelRect = plotView->labelRect("-1", {-1, 0}, AbstractPlotView::RelativePosition::Before, AbstractPlotView::RelativePosition::There);
  plotView->drawLabel(ctx, rect, "-1", labelRect.translatedBy(k_minusOneFineTunePosition), KDColorBlack);
  labelRect = plotView->labelRect("1", {1, 0}, AbstractPlotView::RelativePosition::After, AbstractPlotView::RelativePosition::There);
  plotView->drawLabel(ctx, rect, "1", labelRect.translatedBy(k_oneFineTunePosition), KDColorBlack);
  m_xAxis.drawAxis(plotView, ctx, rect, AbstractPlotView::Axis::Horizontal);
  m_yAxis.drawAxis(plotView, ctx, rect, AbstractPlotView::Axis::Vertical);
}

TrigonometryGraphView::TrigonometryGraphView(TrigonometryModel * model) :
  PlotView(model)
{
  m_model = model;
}

}
