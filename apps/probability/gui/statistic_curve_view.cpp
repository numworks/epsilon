#include "statistic_curve_view.h"

#include <math.h>

#include "probability/app.h"
#include "probability/helpers.h"

namespace Probability {

constexpr KDColor StatisticCurveView::k_backgroundColor;

void StatisticCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), k_backgroundColor);
  drawAxis(ctx, rect, Axis::Horizontal);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, false, false, 0, k_backgroundColor);

  if (App::app()->snapshot()->navigation()->subapp() == Data::SubApp::Tests) {
    drawTest(ctx, rect);
  } else {
    drawInterval(ctx, rect);
  }
}

char * StatisticCurveView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

void StatisticCurveView::drawTest(KDContext * ctx, KDRect rect) const {
  float z = m_statistic->testCriticalValue();
  float zAlpha = m_statistic->zAlpha();
  float min = fminf(z, zAlpha);
  float max = fmaxf(z, zAlpha);
  KDColor middleColor = z > zAlpha ? Escher::Palette::YellowDark : Escher::Palette::GrayMiddle;
  KDColor afterColor = z < zAlpha ? Escher::Palette::YellowDark : Escher::Palette::GrayMiddle;
  drawCartesianCurve(ctx, rect, min, max, evaluateTestAtAbsissa, m_statistic, nullptr, afterColor,
                     true, true, min, max);
  drawCartesianCurve(ctx, rect, max, INFINITY, evaluateTestAtAbsissa, m_statistic, nullptr,
                     middleColor, true, true, max, INFINITY);
  drawCartesianCurve(ctx, rect, -INFINITY, INFINITY, evaluateTestAtAbsissa, m_statistic, nullptr,
                     Escher::Palette::GrayVeryDark);
}

void StatisticCurveView::drawInterval(KDContext * ctx, KDRect rect) const {
  drawCartesianCurve(ctx, rect, -INFINITY, INFINITY, evaluateIntervalAtAbsissa, m_statistic,
                     nullptr, Escher::Palette::YellowDark, true, true,
                     m_statistic->estimate() - m_statistic->marginOfError(),
                     m_statistic->estimate() + m_statistic->marginOfError());
}

}  // namespace Probability
