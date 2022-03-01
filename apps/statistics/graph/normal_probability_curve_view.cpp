#include "normal_probability_curve_view.h"
#include <apps/shared/curve_view.h>
#include <algorithm>
#include <assert.h>

namespace Statistics {

void NormalProbabilityCurveView::drawSeriesCurve(KDContext * ctx, KDRect rect, int series) const {
  PlotCurveView::drawSeriesCurve(ctx, rect, series);

  // Brighter colors for the curves
  constexpr KDColor k_curveColors[Store::k_numberOfSeries] = {
      KDColor::RGB888(255, 204, 204),   // #FFCCCC
      KDColor::RGB888(220, 227, 253),   // #DCE3FD
      KDColor::RGB888(220, 243, 204)};  // #DCF3CC
  KDColor color = k_curveColors[series];

  // Plot the y=(x-mean(X)/sigma(X)) line
  // TODO : Handle frequencies different than 1
  float mean = m_store->mean(series);
  float sigma = m_store->standardDeviation(series);
  float xMin = m_store->minValue(series);
  float xMax = m_store->maxValue(series);
  Shared::CurveView::drawSegment(ctx, rect, xMin, (xMin-mean)/sigma, xMax, (xMax-mean)/sigma, color);
}

}
