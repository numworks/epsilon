#include "histogram_view.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Statistics {

HistogramView::HistogramView(Store * store, int series, Shared::CurveViewRange * curveViewRange) :
  HorizontallyLabeledCurveView(curveViewRange, nullptr, nullptr, nullptr),
  m_store(store),
  m_highlightedBarStart(NAN),
  m_highlightedBarEnd(NAN),
  m_series(series),
  m_displayLabels(true)
{
}

void HistogramView::reload(bool resetInterrupted, bool force) {
  CurveView::reload(resetInterrupted, force);
  markRectAsDirty(bounds());
}

void HistogramView::reloadSelectedBar() {
  CurveView::reload();
  float pixelLowerBound = floatToPixel(Axis::Horizontal, m_highlightedBarStart)-2;
  float pixelUpperBound = floatToPixel(Axis::Horizontal, m_highlightedBarEnd)+2;
  /* We deliberately do not mark as dirty the frame of the banner view to avoid
   *unpleasant blinking of the drawing of the banner view. */
  KDRect dirtyZone(KDRect(pixelLowerBound, 0, pixelUpperBound-pixelLowerBound,
    bounds().height()));
  markRectAsDirty(dirtyZone);
}

void HistogramView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxis(ctx, rect, Axis::Horizontal);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, !m_displayLabels);
  /* We memoize the maximal bar size to avoid recomputing it at every call to
   * EvaluateHistogramAtAbscissa() */
  float totalSize = m_store->maxHeightOfBar(m_series);
  float context[] = {totalSize, static_cast<float>(m_series)};
  if (isMainViewSelected()) {
    drawHistogram(ctx, rect, EvaluateHistogramAtAbscissa, m_store, context, m_store->firstDrawnBarAbscissa(), m_store->barWidth(), true, DoublePairStore::colorLightOfSeriesAtIndex(m_series), k_selectedBarColor, 1, Shared::DoublePairStore::colorOfSeriesAtIndex(m_series), m_highlightedBarStart, m_highlightedBarEnd);
  } else {
    drawHistogram(ctx, rect, EvaluateHistogramAtAbscissa, m_store, context, m_store->firstDrawnBarAbscissa(), m_store->barWidth(), true, k_notSelectedHistogramColor, k_selectedBarColor, 1, k_notSelectedHistogramColor);
  }
}

void HistogramView::setHighlight(float start, float end) {
  if (m_highlightedBarStart != start || m_highlightedBarEnd != end) {
    reloadSelectedBar();
    m_highlightedBarStart = start;
    m_highlightedBarEnd = end;
    reloadSelectedBar();
  }
}

float HistogramView::EvaluateHistogramAtAbscissa(float abscissa, void * model, void * context) {
  Store * store = (Store *)model;
  float totalSize = ((float *)context)[0];
  int series = ((float *)context)[1];
  return store->heightOfBarAtValue(series, abscissa) / totalSize;
}

}
