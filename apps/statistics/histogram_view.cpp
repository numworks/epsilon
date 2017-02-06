#include "histogram_view.h"
#include <assert.h>
#include <math.h>

using namespace Shared;

namespace Statistics {

HistogramView::HistogramView(Store * store, BannerView * bannerView) :
  CurveView(store, nullptr, bannerView, nullptr),
  m_store(store),
  m_highlightedBarStart(NAN),
  m_highlightedBarEnd(NAN)
{
}

void HistogramView::reload() {
  CurveView::reload();
  float pixelLowerBound = floatToPixel(Axis::Horizontal, m_highlightedBarStart)-2;
  float pixelUpperBound = floatToPixel(Axis::Horizontal, m_highlightedBarEnd)+2;
  KDRect dirtyZone(KDRect(pixelLowerBound, 0, pixelUpperBound-pixelLowerBound,
    bounds().height()));
  markRectAsDirty(dirtyZone);
}

void HistogramView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawLabels(ctx, rect, Axis::Horizontal, false);
  if (isMainViewSelected()) {
    drawHistogram(ctx, rect, nullptr, m_store->firstDrawnBarAbscissa(), m_store->barWidth(), true, Palette::YellowDark, Palette::Select,
      m_highlightedBarStart, m_highlightedBarEnd);
  } else {
    drawHistogram(ctx, rect, nullptr, m_store->firstDrawnBarAbscissa(), m_store->barWidth(), true, Palette::YellowDark, Palette::Select);
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
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

float HistogramView::evaluateModelWithParameter(Model * curve, float t) const {
  return m_store->heightOfBarAtValue(t)/m_store->sumOfColumn(1);
}

}
