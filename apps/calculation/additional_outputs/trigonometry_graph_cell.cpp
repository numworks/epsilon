#include "trigonometry_graph_cell.h"

using namespace Shared;
using namespace Poincare;

namespace Calculation {

TrigonometryGraphView::TrigonometryGraphView(TrigonometryModel * model) :
  CurveView(model),
  m_model(model)
{
}

void TrigonometryGraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  // Draw the circle
  drawCurve(ctx, rect, 0, 2.0f*M_PI, M_PI/180.0f, [](float t, void * model, void * context) {
      return Poincare::Coordinate2D<float>(std::cos(t), std::sin(t));
    }, nullptr, nullptr, true, Palette::GreyDark, false);
  // Draw dashed segment to indicate sine and cosine
  drawSegment(ctx, rect, Axis::Vertical, std::cos(m_model->angle()), 0.0f, std::sin(m_model->angle()), Palette::Red, 1, 3);
  drawSegment(ctx, rect, Axis::Horizontal, std::sin(m_model->angle()), 0.0f, std::cos(m_model->angle()), Palette::Red, 1, 3);
  // Draw angle position on the circle
  drawDot(ctx, rect, std::cos(m_model->angle()), std::sin(m_model->angle()), Palette::Red);
  // Draw labels
  drawLabel(ctx, rect, Axis::Vertical, std::sin(m_model->angle()), "sin(θ)", Palette::Red);
  drawLabel(ctx, rect, Axis::Horizontal, std::cos(m_model->angle()), "cos(θ)", Palette::Red);
}

}
