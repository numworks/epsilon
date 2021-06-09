#include "statistic_curve_view.h"

namespace Probability {

constexpr KDColor StatisticCurveView::k_backgroundColor;

void StatisticCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), k_backgroundColor);
  drawAxis(ctx, rect, Axis::Horizontal);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, false, false, 0, k_backgroundColor);

  float z = m_statistic->testCriticalValue();
  float p = m_statistic->pValue();
  float min = fmin(z, p);
  float max = fmax(z, p);
  KDColor middleColor = z > p ? Escher::Palette::YellowDark : Escher::Palette::GrayMiddle;
  KDColor afterColor = z < p ? Escher::Palette::YellowDark : Escher::Palette::GrayMiddle;
  drawCartesianCurve(ctx, rect, min, max, evaluateAtAbsissa, m_statistic, nullptr, afterColor, true, true, min, max);
  drawCartesianCurve(ctx, rect, max, INFINITY, evaluateAtAbsissa, m_statistic, nullptr, middleColor, true, true, max, INFINITY);
  drawCartesianCurve(ctx, rect, -INFINITY, INFINITY, evaluateAtAbsissa, m_statistic, nullptr, Escher::Palette::GrayVeryDark);
}

char * StatisticCurveView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

} // namespace Probability
