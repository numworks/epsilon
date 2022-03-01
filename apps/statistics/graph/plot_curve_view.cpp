#include "plot_curve_view.h"
#include <apps/shared/curve_view.h>
#include <algorithm>
#include <assert.h>

namespace Statistics {

PlotCurveView::PlotCurveView(Shared::CurveViewRange * curveViewRange,
                             Shared::CurveViewCursor * curveViewCursor,
                             Shared::CursorView * cursorView,
                             Store * store) :
    // No banners to display
    Shared::LabeledCurveView(curveViewRange, curveViewCursor, nullptr, cursorView, false),
    m_store(store) {
}

void PlotCurveView::moveCursorTo(int i, int series) {
  // TODO : Add continuous curve scrolling
  int sortedIndex[Store::k_maxNumberOfPairs];
  m_store->buildSortedIndex(series, sortedIndex);
  // Compute coordinates
  double x = valueAtIndex(series, sortedIndex, i);
  double y = resultAtIndex(series, sortedIndex, i);
  m_curveViewCursor->moveTo(x, x, y);
  reload();
}

void PlotCurveView::drawSeriesCurve(KDContext * ctx, KDRect rect, int series) const {
  int sortedIndex[Store::k_maxNumberOfPairs];
  m_store->buildSortedIndex(series, sortedIndex);
  int numberOfPairs = totalValues(series, sortedIndex);
  // Draw and connect each points
  KDColor color = Store::colorLightOfSeriesAtIndex(series);
  double previousX, previousY;
  for (size_t i = 0; i < numberOfPairs; i++) {
    double x = valueAtIndex(series, sortedIndex, i);
    double y = resultAtIndex(series, sortedIndex, i);
    Shared::CurveView::drawDot(ctx, rect, x, y, color);
    if (connectPoints() && i > 0) {
      Shared::CurveView::drawSegment(ctx, rect, x, y, previousX, previousY, color);
    }
    previousX = x;
    previousY = y;
  }
}

void PlotCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  simpleDrawBothAxesLabels(ctx, rect);
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (m_store->seriesIsValid(i)) {
      drawSeriesCurve(ctx, rect, i);
    }
  }
}

}
