#include "statistic_curve_view.h"

namespace Probability {

constexpr KDColor StatisticCurveView::k_backgroundColor;

void StatisticCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), k_backgroundColor);
  drawAxis(ctx, rect, Axis::Horizontal);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, false, false, 0, k_backgroundColor);
  drawCartesianCurve(ctx, rect, -INFINITY, INFINITY, evaluateAtAbsissa, m_statistic, nullptr, KDColorBlue);
}

char * StatisticCurveView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

} // namespace Probability
