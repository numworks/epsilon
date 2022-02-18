#include "frequency_curve_view.h"
#include <apps/shared/curve_view.h>
#include <algorithm>
#include <assert.h>

namespace Statistics {

FrequencyCurveView::FrequencyCurveView(Shared::CurveViewRange * curveViewRange,
                                       Shared::CurveViewCursor * curveViewCursor,
                                       Shared::CursorView * cursorView,
                                       Store * store) :
    // No banners to display
    Shared::LabeledCurveView(curveViewRange, curveViewCursor, nullptr, cursorView, false),
    m_store(store) {
}

void FrequencyCurveView::moveCursorTo(int i, int series) {
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
  double y = m_store->cumulatedFrequencyAtSortedIndex(series, sortedIndex, i);
  m_curveViewCursor->moveTo(x, x, y);
  reload();
}

void FrequencyCurveView::drawSeriesCumulatedFrequencyCurve(KDContext * ctx, KDRect rect, int series) const {
  // TODO : Factorize sortedIndex part
  int sortedIndex[Store::k_maxNumberOfPairs];
  int numberOfPairs = m_store->numberOfPairsOfSeries(series);
  for (int i = 0; i < numberOfPairs; i++) {
    sortedIndex[i] = i;
  }
  m_store->sortIndex(series, sortedIndex, 0, numberOfPairs);

  // Draw and connect each points
  KDColor color = Store::colorLightOfSeriesAtIndex(series);
  double previousX, previousY;
  for (size_t i = 0; i < numberOfPairs; i++) {
    double x = m_store->get(series, 0, sortedIndex[i]);
    double y = m_store->cumulatedFrequencyAtSortedIndex(series, sortedIndex, i);
    Shared::CurveView::drawDot(ctx, rect, x, y, color);
    if (i > 0) {
      Shared::CurveView::drawSegment(ctx, rect, x, y, previousX, previousY, color);
    }
    previousX = x;
    previousY = y;
  }
}

// TODO : Always enforce 20%, 40%, 60%, 80% and 100% labels
void FrequencyCurveView::appendLabelSuffix(Axis axis, char * labelBuffer, int maxSize, int glyphLength, int maxGlyphLength) {
  if (axis == Axis::Horizontal) {
    return;
  }
  int length = strlen(labelBuffer);
  if (glyphLength >= maxGlyphLength || length + 1 >= maxSize) {
    // Suffix must fit, empty the label.
    labelBuffer[0] = 0;
    return;
  }
  assert(labelBuffer[length-1] != '%');
  labelBuffer[length] = '%';
  labelBuffer[length+1] = 0;
}

void FrequencyCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  simpleDrawBothAxesLabels(ctx, rect);
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (!m_store->seriesIsEmpty(i)) {
      drawSeriesCumulatedFrequencyCurve(ctx, rect, i);
    }
  }
}

}
