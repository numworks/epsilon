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
  float sin = std::sin(m_model->angle());
  float cos = std::cos(m_model->angle());
  float tan = sin/cos;
  float sec = 1/cos;
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  // Draw the circle
  drawCurve(ctx, rect, 0.0f, 2.0f*M_PI, M_PI/180.0f, [](float t, void * model, void * context) {
      return Poincare::Coordinate2D<float>(std::cos(t), std::sin(t));
    }, nullptr, nullptr, true, Palette::GrayDark, false);
  // Draw dashed segment to indicate sine, cosine, tangent and secant
  drawHorizontalOrVerticalSegment(ctx, rect, Axis::Vertical, cos, 0.0f, sin, Palette::Red, 1, 3);
  drawHorizontalOrVerticalSegment(ctx, rect, Axis::Horizontal, sin, 0.0f, cos, Palette::Red, 1, 3);
  //drawSegment(ctx, rect, cos, sin, sec, 0.0f, Palette::Red, false, true);
  // Draw angle position on the circle
  drawDot(ctx, rect, cos, sin, Palette::Red, Size::Large);
  // Draw graduations
  drawLabelsAndGraduations(ctx, rect, Axis::Vertical, false, true);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, true);
  // Draw labels
  drawLabel(ctx, rect, 0.0f, sin, "sin(θ)", Palette::Red, cos >= 0.0f ? CurveView::RelativePosition::Before : CurveView::RelativePosition::After, CurveView::RelativePosition::None);
  drawLabel(ctx, rect, cos, 0.0f, "cos(θ)", Palette::Red, CurveView::RelativePosition::None, sin >= 0.0f ? CurveView::RelativePosition::Before : CurveView::RelativePosition::After);
}

}
