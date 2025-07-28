#include "trigonometry_graph_cell.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Calculation {

void TrigonometryGraphPolicy::drawPlot(const AbstractPlotView* plotView,
                                       KDContext* ctx, KDRect rect) const {
  assert(m_model && plotView);

  float angle = m_model->angle();
  assert(std::isfinite(angle));
  assert(0 <= angle && angle < 2 * M_PI + OMG::Float::EpsilonLax<float>());
  if (!std::isfinite(angle)) {
    /* This is defensive coding to avoid looping infinitely in drawArcOfEllipse
     * when angle is not finite. This should not happen since some asserts
     * prevent it, but history has shown that we always end up with
     * trigonometric additional results that trigger these asserts, and loop
     * infinitely in release. This code should be removed once the additional
     * results are refactored. */
    return;
  }
  float s = std::sin(angle);
  float c = std::cos(angle);

  // - Draw sine and cosine projections
  plotView->drawDashedStraightSegment(ctx, rect, OMG::Axis::Horizontal, s, 0.f,
                                      c, Palette::Red);
  plotView->drawDashedStraightSegment(ctx, rect, OMG::Axis::Vertical, c, 0.f, s,
                                      Palette::Red);
  plotView->drawSegment(ctx, rect, {0.f, 0.f}, {c, s}, Palette::Red);

  // - Draw angle position on the circle
  plotView->drawDot(ctx, rect, Dots::Size::Large, Coordinate2D<float>(c, s),
                    Palette::Red);

  // - Draw angle arc and label
  constexpr float k_arcRatio = 0.2;
  constexpr float k_labelRatio = 0.32;
  bool labelOnLine =
      std::fabs(angle) < M_PI / 30.f || std::fabs(M_PI - angle) < M_PI / 30.f;
  plotView->drawLabel(
      ctx, rect, "θ",
      {k_labelRatio * std::cos(angle / 2), k_labelRatio * std::sin(angle / 2)},
      AbstractPlotView::RelativePosition::There,
      labelOnLine ? AbstractPlotView::RelativePosition::Before
                  : AbstractPlotView::RelativePosition::There,
      Palette::Red);
  // Draw the arc
  drawArcOfEllipse(plotView, ctx, rect, {0.f, 0.f}, k_arcRatio, k_arcRatio,
                   0.0f, angle, Palette::Red);

  // - Draw "sin(θ)" and "cos(θ)" labels
  plotView->drawLabel(ctx, rect, "sin(θ)", Coordinate2D<float>(0.f, s),
                      c >= 0.f ? AbstractPlotView::RelativePosition::Before
                               : AbstractPlotView::RelativePosition::After,
                      AbstractPlotView::RelativePosition::There, Palette::Red);
  plotView->drawLabel(ctx, rect, "cos(θ)", Coordinate2D<float>(c, 0.f),
                      AbstractPlotView::RelativePosition::There,
                      s >= 0.f ? AbstractPlotView::RelativePosition::After
                               : AbstractPlotView::RelativePosition::Before,
                      Palette::Red);
}

void UnitCircle::drawAxesAndGrid(const AbstractPlotView* plotView,
                                 KDContext* ctx, KDRect rect) const {
  PlotPolicy::WithPolarGrid::DrawGrid(plotView, ctx, rect);
  plotView->drawCircle(ctx, rect, {0.f, 0.f}, 1.f, KDColorBlack);
  // Labels should be as close to the axes as possible so we place them manually
  KDRect labelRect = plotView->labelRect(
      "-1", {-1, 0}, AbstractPlotView::RelativePosition::Before,
      AbstractPlotView::RelativePosition::There);
  plotView->drawLabel(ctx, rect, "-1",
                      labelRect.translatedBy(k_minusOneFineTunePosition),
                      KDColorBlack);
  labelRect = plotView->labelRect("1", {1, 0},
                                  AbstractPlotView::RelativePosition::After,
                                  AbstractPlotView::RelativePosition::There);
  plotView->drawLabel(ctx, rect, "1",
                      labelRect.translatedBy(k_oneFineTunePosition),
                      KDColorBlack);
  m_xAxis.drawAxis(plotView, ctx, rect, OMG::Axis::Horizontal);
  m_yAxis.drawAxis(plotView, ctx, rect, OMG::Axis::Vertical);
}

TrigonometryGraphView::TrigonometryGraphView(TrigonometryModel* model)
    : PlotView(model) {
  m_model = model;
}

}  // namespace Calculation
