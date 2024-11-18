#include "histogram_view.h"

#include <assert.h>

#include <cmath>

using namespace Shared;

namespace Statistics {

// HistogramPlotPolicy

static double histogramLevels(double x, void* model, void* context) {
  Store* store = reinterpret_cast<Store*>(model);
  double* parameters = reinterpret_cast<double*>(context);
  double maxSize = parameters[0];
  double series = parameters[1];
  assert(maxSize >= 0);
  return maxSize == 0 ? 0 : store->heightOfBarAtValue(series, x) / maxSize;
}

static bool barIsHighlighted(double x, void* model, void* context) {
  double* parameters = reinterpret_cast<double*>(context);
  double start = parameters[2];
  double end = parameters[3];
  return start <= x && x < end;
}

void HistogramPlotPolicy::drawPlot(const Shared::AbstractPlotView* plotView,
                                   KDContext* ctx, KDRect rect) const {
  double context[] = {m_store->maxHeightOfBar(m_series),
                      static_cast<double>(m_series), m_highlightedBarStart,
                      m_highlightedBarEnd};

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

  HistogramDrawing histogram(histogramLevels, m_store, context, highlights,
                             m_store->firstDrawnBarAbscissa(),
                             m_store->barWidth(), true, true, color,
                             k_selectedBarColor, borderColor);
  histogram.draw(plotView, ctx, rect);
}

// HistogramView

HistogramView::HistogramView(Store* store, int series,
                             Shared::CurveViewRange* range)
    : PlotView(range) {
  // HistogramPlotPolicy
  m_store = store;
  m_series = series;
  m_highlightedBarStart = NAN;
  m_highlightedBarEnd = NAN;
}

void HistogramView::reload(bool resetInterruption, bool force) {
  AbstractPlotView::reload(resetInterruption, force);
  markWholeFrameAsDirty();
}

void HistogramView::setHighlight(double start, double end) {
  if (m_highlightedBarStart != start || m_highlightedBarEnd != end) {
    reloadSelectedBar();
    m_highlightedBarStart = start;
    m_highlightedBarEnd = end;
    reloadSelectedBar();
  }
}

void HistogramView::reloadSelectedBar() {
  AbstractPlotView::reload();
  KDCoordinate barInteriorStartPixelIndex =
      floatToKDCoordinatePixel(OMG::Axis::Horizontal, m_highlightedBarStart) +
      PlotPolicy::WithHistogram::k_borderWidth;
  KDCoordinate barInteriorEndPixelIndex =
      floatToKDCoordinatePixel(OMG::Axis::Horizontal, m_highlightedBarEnd) -
      PlotPolicy::WithHistogram::k_borderWidth;
  /* We deliberately do not mark as dirty the frame of the banner view to avoid
   * unpleasant blinking of the drawing of the banner view. */
  markRectAsDirty(
      KDRect(barInteriorStartPixelIndex, 0,
             barInteriorEndPixelIndex - barInteriorStartPixelIndex + 1,
             bounds().height()));
}

}  // namespace Statistics
