#include "trigonometry_graph_cell.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Calculation {

TrigonometryGraphView::TrigonometryGraphView(TrigonometryModel * model) :
  CurveView(model),
  m_model(model)
{
}

void TrigonometryGraphView::drawRect(KDContext * ctx, KDRect rect) const {
  float s = std::sin(m_model->angle());
  float c = std::cos(m_model->angle());
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  // Draw the circle
  drawCurve(ctx, rect, 0.0f, 2.0f*M_PI, M_PI/180.0f, [](float t, void * model, void * context) {
      return Poincare::Coordinate2D<float>(std::cos(t), std::sin(t));
    }, nullptr, nullptr, true, Palette::GrayDark, CurveView::DefaultEvaluateDiscontinuityBetweenFloatValues, false);
  // Draw dashed segment to indicate sine and cosine
  drawHorizontalOrVerticalSegment(ctx, rect, Axis::Vertical, c, 0.0f, s, Palette::Red, 1, 3);
  drawHorizontalOrVerticalSegment(ctx, rect, Axis::Horizontal, s, 0.0f, c, Palette::Red, 1, 3);
  // Draw angle position on the circle
  drawDot(ctx, rect, c, s, Palette::Red, Size::Large);
  // Draw graduations
  drawLabelsAndGraduations(ctx, rect, Axis::Vertical, false, true);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, true);
  // Draw labels
  drawLabel(ctx, rect, 0.0f, s, "sin(θ)", Palette::Red, c >= 0.0f ? CurveView::RelativePosition::Before : CurveView::RelativePosition::After, CurveView::RelativePosition::None);
  drawLabel(ctx, rect, c, 0.0f, "cos(θ)", Palette::Red, CurveView::RelativePosition::None, s >= 0.0f ? CurveView::RelativePosition::Before : CurveView::RelativePosition::After);
}

}
