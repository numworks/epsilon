#include "histogram_view.h"
#include "histogram_controller.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Statistics {

HistogramView::HistogramView(HistogramController * controller, Store * store, int series, Shared::BannerView * bannerView, KDColor selectedHistogramColor, KDColor notSelectedHistogramColor, KDColor selectedBarColor) :
  CurveView(store, nullptr, bannerView, nullptr),
  m_controller(controller),
  m_store(store),
  m_labels{},
  m_highlightedBarStart(NAN),
  m_highlightedBarEnd(NAN),
  m_series(series),
  m_selectedHistogramColor(selectedHistogramColor),
  m_notSelectedHistogramColor(notSelectedHistogramColor),
  m_selectedBarColor(selectedBarColor)
{
}

void HistogramView::reload() {
  CurveView::reload();
  /* We deliberately do not mark as dirty the frame of the banner view to avoid
   *unpleasant blinking of the drawing of the banner view. */
  KDRect dirtyZone(KDRect(0, 0, bounds().width(), bounds().height() - (displayBannerView() ? m_bannerView->bounds().height() : 0)));
  markRectAsDirty(dirtyZone);
}

void HistogramView::drawRect(KDContext * ctx, KDRect rect) const {
  m_controller->setCurrentDrawnSeries(m_series);
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawLabels(ctx, rect, Axis::Horizontal, false);
  /* We memoize the total size to avoid recomputing it in double precision at
   * every call to EvaluateHistogramAtAbscissa() */
  float totalSize = m_store->sumOfOccurrences(m_series);
  float context[] = {totalSize, static_cast<float>(m_series)};
  if (isMainViewSelected()) {
    drawHistogram(ctx, rect, EvaluateHistogramAtAbscissa, m_store, context, m_store->firstDrawnBarAbscissa(), m_store->barWidth(), true, m_selectedHistogramColor, m_selectedBarColor, m_highlightedBarStart, m_highlightedBarEnd);
  } else {
    drawHistogram(ctx, rect, EvaluateHistogramAtAbscissa, m_store, context, m_store->firstDrawnBarAbscissa(), m_store->barWidth(), true, m_notSelectedHistogramColor, m_selectedBarColor);
  }
}

void HistogramView::setHighlight(float start, float end) {
  if (m_highlightedBarStart != start || m_highlightedBarEnd != end) {
    reload();
    m_highlightedBarStart = start;
    m_highlightedBarEnd = end;
    reload();
  }
}

char * HistogramView::label(Axis axis, int index) const {
  return axis == Axis::Vertical ? nullptr : (char *)m_labels[index];
}

float HistogramView::EvaluateHistogramAtAbscissa(float abscissa, void * model, void * context) {
  Store * store = (Store *)model;
  float totalSize = ((float *)context)[0];
  int series = ((float *)context)[1];
  return store->heightOfBarAtValue(series, abscissa)/(totalSize);
}

}
