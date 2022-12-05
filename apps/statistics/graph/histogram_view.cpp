#include "histogram_view.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Statistics {

// HistogramPlotPolicy

static float histogramLevels(float x, void * model, void * context) {
  Store * store = reinterpret_cast<Store *>(model);
  float * parameters = reinterpret_cast<float *>(context);
  float maxSize = parameters[0];
  float series = parameters[1];
  return store->heightOfBarAtValue(series, x) / maxSize;
}

static bool barIsHighlighted(float x, void * model, void * context) {
  float * parameters = reinterpret_cast<float *>(context);
  float start = parameters[2];
  float end = parameters[3];
  return start <= x && x < end;
}

void HistogramPlotPolicy::drawPlot(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {

  float context[] = { static_cast<float>(m_store->maxHeightOfBar(m_series)), static_cast<float>(m_series), m_highlightedBarStart, m_highlightedBarEnd };

  KDColor color, borderColor;
  HighlightTest highlights = nullptr;
  if (plotView->hasFocus()) {
    color = DoublePairStore::colorLightOfSeriesAtIndex(m_series);
    borderColor = DoublePairStore::colorOfSeriesAtIndex(m_series);
    highlights = &barIsHighlighted;
  } else {
    color = k_notSelectedHistogramColor;
    borderColor = k_notSelectedHistogramBorderColor;
  }

  HistogramDrawing histogram(histogramLevels, m_store, context, m_store->firstDrawnBarAbscissa(), m_store->barWidth(), color, true);
  histogram.setBorderOptions(k_borderWidth, borderColor);
  histogram.setHighlightOptions(highlights, k_selectedBarColor);
  histogram.draw(plotView, ctx, rect);
}

// HistogramView

HistogramView::HistogramView(Store * store, int series, Shared::CurveViewRange * range) :
  PlotView(range)
{
  // HistogramPlotPolicy
  m_store = store;
  m_series = series;
}

void HistogramView::reload(bool resetInterruption, bool force) {
  AbstractPlotView::reload(resetInterruption, force);
  markRectAsDirty(bounds());
}

void HistogramView::setHighlight(float start, float end) {
  if (m_highlightedBarStart != start || m_highlightedBarEnd != end) {
    reloadSelectedBar();
    m_highlightedBarStart = start;
    m_highlightedBarEnd = end;
    reloadSelectedBar();
  }
}

void HistogramView::reloadSelectedBar() {
  AbstractPlotView::reload();
  float pixelLowerBound = floatToPixel(Axis::Horizontal, m_highlightedBarStart) - 2;
  float pixelUpperBound = floatToPixel(Axis::Horizontal, m_highlightedBarEnd) + 2;
  /* We deliberately do not mark as dirty the frame of the banner view to avoid
   *unpleasant blinking of the drawing of the banner view. */
  KDRect dirtyZone(pixelLowerBound, 0, pixelUpperBound-pixelLowerBound, bounds().height());
  markRectAsDirty(dirtyZone);
}


}