#include "normal_probability_curve_view.h"
#include <apps/shared/curve_view.h>
#include <algorithm>
#include <assert.h>

namespace Statistics {

NormalProbabilityCurveView::NormalProbabilityCurveView(Shared::CurveViewRange * curveViewRange,
                                       Shared::CurveViewCursor * curveViewCursor,
                                       Shared::CursorView * cursorView,
                                       Store * store) :
    // No banners to display
    Shared::LabeledCurveView(curveViewRange, curveViewCursor, nullptr, cursorView, false),
    m_store(store) {
}

void NormalProbabilityCurveView::moveCursorTo(int i, int series) {
  // TODO : Add continuous curve scrolling
  // TODO : Factorize sortedIndex part
  int sortedIndex[Store::k_maxNumberOfPairs];
  int numberOfPairs = m_store->numberOfPairsOfSeries(series);
  for (int i = 0; i < numberOfPairs; i++) {
    sortedIndex[i] = i;
  }
  m_store->sortIndex(series, sortedIndex, 0, numberOfPairs);
  // Compute coordinates
  double x = m_store->get(series, 0, sortedIndex[i]);
  double y = m_store->zScoreAtSortedIndex(series, sortedIndex, i);
  m_curveViewCursor->moveTo(x, x, y);
  reload();
}

void NormalProbabilityCurveView::drawSeriesCumulatedNormalProbabilityCurve(KDContext * ctx, KDRect rect, int series) const {
  // TODO : Factorize sortedIndex part
  int sortedIndex[Store::k_maxNumberOfPairs];
  int numberOfPairs = m_store->numberOfPairsOfSeries(series);
  for (int i = 0; i < numberOfPairs; i++) {
    sortedIndex[i] = i;
  }
  m_store->sortIndex(series, sortedIndex, 0, numberOfPairs);

  // Draw each points
  KDColor color = Store::colorLightOfSeriesAtIndex(series);
  double previousX, previousY;
  for (size_t i = 0; i < numberOfPairs; i++) {
    double x = m_store->get(series, 0, sortedIndex[i]);
    double y = m_store->zScoreAtSortedIndex(series, sortedIndex, i);
    Shared::CurveView::drawDot(ctx, rect, x, y, color);
    previousX = x;
    previousY = y;
  }

  // Brighter colors for the curves
  constexpr KDColor k_curveColors[Store::k_numberOfSeries] = {
      KDColor::RGB888(255, 204, 204),   // #FFCCCC
      KDColor::RGB888(220, 227, 253),   // #DCE3FD
      KDColor::RGB888(220, 243, 204)};  // #DCF3CC
  color = k_curveColors[series];

  // Plot the y=(x-mean(X)/sigma(X)) line
  // TODO : Handle frequencies different than 1
  float mean = m_store->mean(series);
  float sigma = m_store->standardDeviation(series);
  float xMin = m_store->minValue(series);
  float xMax = m_store->maxValue(series);
  Shared::CurveView::drawSegment(ctx, rect, xMin, (xMin-mean)/sigma, xMax, (xMax-mean)/sigma, color);
}

void NormalProbabilityCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  simpleDrawBothAxesLabels(ctx, rect);
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (!m_store->seriesIsEmpty(i)) {
      drawSeriesCumulatedNormalProbabilityCurve(ctx, rect, i);
    }
  }
}

}
