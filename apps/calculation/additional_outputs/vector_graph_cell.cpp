#include "vector_graph_cell.h"
#include "kandinsky/color.h"
#include "vector_model.h"
#include "escher/palette.h"
#include <cmath>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Calculation {

VectorGraphView::VectorGraphView(VectorModel * model) :
  LabeledCurveView(model),
  m_model(model)
{
}

void VectorGraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  // Draw graduations
  drawLabelsAndGraduations(ctx, rect, Axis::Vertical, false, false);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, false);
  float x = m_model->vectorX();
  float y = m_model->vectorY();
  const float length = std::sqrt(std::pow(x,2) + std::pow(y,2));
  const float angle = y>=0 ? std::acos(x / length) : 2*M_PI - std::acos(x / length);
  // Draw label
  bool labelOnLine = std::abs(angle) < M_PI/30.f || std::abs(M_PI - angle) < M_PI/30.f;
  float labelDistance = pixelLengthToFloatLength(Axis::Horizontal, k_labelDistanceInPixels);
  drawLabel(ctx, rect, labelDistance * std::cos(angle/2), labelDistance * std::sin(angle/2), "θ", Palette::GrayDark, CurveView::RelativePosition::None, labelOnLine ? CurveView::RelativePosition::Before : CurveView::RelativePosition::None);
  // Draw arc
  drawArc(ctx, rect, 0.0f, angle, M_PI/30.0f, pixelLengthToFloatLength(Axis::Horizontal, m_model->arcRadiusInPixels()), Palette::GrayDark, false);
  // Draw arrow
  drawSegment(ctx, rect, 0., 0., x, y, Palette::Red, false);
  drawArrow(ctx, rect, x, y, x, y, Palette::Red, pixelLengthToFloatLength(Axis::Horizontal, k_arrowSizeInPixels), false);
}

void VectorGraphCell::reloadCell() {
  m_view.reload();
}

}
