#include "trigonometry_graph_cell.h"
#include <escher/palette.h>

using namespace Shared;
using namespace Poincare;

namespace Calculation {

TrigonometryGraphView::TrigonometryGraphView(TrigonometryModel * model) :
  CurveView(model),
  m_model(model)
{
}

void TrigonometryGraphView::drawRect(KDContext * ctx, KDRect rect) const {
  float s = std::sin(m_model->angle());
  float c = std::cos(m_model->angle());
  ctx->fillRect(rect, Palette::BackgroundApps);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  // Draw the circle
  drawCurve(ctx, rect, 0.0f, 2.0f*M_PI, M_PI/180.0f, [](float t, void * model, void * context) {
      return Poincare::Coordinate2D<float>(std::cos(t), std::sin(t));
    }, nullptr, nullptr, true, Palette::SecondaryText, false);
  // Draw dashed segment to indicate sine and cosine
  drawHorizontalOrVerticalSegment(ctx, rect, Axis::Vertical, c, 0.0f, s, Palette::CalculationTrigoAndComplexForeground, 1, 3);
  drawHorizontalOrVerticalSegment(ctx, rect, Axis::Horizontal, s, 0.0f, c, Palette::CalculationTrigoAndComplexForeground, 1, 3);
  // Draw angle position on the circle
  drawDot(ctx, rect, c, s, Palette::CalculationTrigoAndComplexForeground, Size::Large);
  // Draw graduations
  drawLabelsAndGraduations(ctx, rect, Axis::Vertical, false, true);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, true);
  // Draw labels
  drawLabel(ctx, rect, 0.0f, s, "sin(θ)", Palette::CalculationTrigoAndComplexForeground, c >= 0.0f ? CurveView::RelativePosition::Before : CurveView::RelativePosition::After, CurveView::RelativePosition::None);
  drawLabel(ctx, rect, c, 0.0f, "cos(θ)", Palette::CalculationTrigoAndComplexForeground, CurveView::RelativePosition::None, s >= 0.0f ? CurveView::RelativePosition::Before : CurveView::RelativePosition::After);
}

}
