#include "plot_curve_view.h"
#include "plot_controller.h"
#include <apps/shared/curve_view.h>
#include <algorithm>
#include <assert.h>

namespace Statistics {

PlotCurveView::PlotCurveView(Shared::CurveViewRange * curveViewRange,
                             Shared::CurveViewCursor * curveViewCursor,
                             Shared::CursorView * cursorView,
                             PlotController * plotController) :
    // No banners to display
    Shared::LabeledCurveView(curveViewRange, curveViewCursor, nullptr, cursorView),
    m_plotController(plotController) {
}

void PlotCurveView::drawSeriesCurve(KDContext * ctx, KDRect rect, int series) const {
  int numberOfPairs = m_plotController->totalValues(series);
  // Draw and connect each points
  KDColor color = Store::colorOfSeriesAtIndex(series);
  double previousX = NAN, previousY = NAN;
  for (int i = 0; i < numberOfPairs; i++) {
    double x = m_plotController->valueAtIndex(series, i);
    double y = m_plotController->resultAtIndex(series, i);
    Shared::CurveView::drawDot(ctx, rect, x, y, color);
    if (m_plotController->connectPoints() && i > 0) {
      Shared::CurveView::drawSegment(ctx, rect, x, y, previousX, previousY, color);
    }
    previousX = x;
    previousY = y;
  }
  float x, y, u, v;
  if (m_plotController->drawSeriesZScoreLine(series, &x, &y, &u, &v)) {
    // Using brighter colors for the lines
    color = Store::colorLightOfSeriesAtIndex(series);
    Shared::CurveView::drawSegment(ctx, rect, x, y, u, v, color);
  }
}

void PlotCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  simpleDrawBothAxesLabels(ctx, rect);
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (m_plotController->seriesIsValid(i)) {
      drawSeriesCurve(ctx, rect, i);
    }
  }
}

void PlotCurveView::appendLabelSuffix(Axis axis, char * labelBuffer, int maxSize, int glyphLength, int maxGlyphLength) {
  m_plotController->appendLabelSuffix(axis, labelBuffer, maxSize, glyphLength, maxGlyphLength);
}

float PlotCurveView::labelStepFactor(Axis axis) const {
  return m_plotController->labelStepMultiplicator(axis);
}

}
