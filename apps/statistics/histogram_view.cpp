#include "histogram_view.h"
#include <assert.h>
#include <float.h>

namespace Statistics {

HistogramView::HistogramView(Data * data) :
  CurveView(data),
  m_data(data),
  m_selectedBins(true)
{
}

void HistogramView::reload() {
  // TODO: optimize dirtiness
  markRectAsDirty(bounds());
  computeLabels(Axis::Horizontal);
}

bool HistogramView::selectedBins() {
  return m_selectedBins;
}

void HistogramView::selectBins(bool selectedBins) {
  if (m_selectedBins != selectedBins) {
    m_selectedBins = selectedBins;
    markRectAsDirty(bounds());
  }
}

void HistogramView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(Axis::Horizontal, ctx, rect);
  drawLabels(Axis::Horizontal, true, ctx, rect);
  if (m_selectedBins) {
    drawHistogram(m_data->binWidth(), KDColorBlack, ctx, rect, KDColorRed, m_data->selectedBin());
  } else {
    drawHistogram(m_data->binWidth(), KDColorBlack, ctx, rect, KDColorRed, FLT_MAX);
  }
}

char * HistogramView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

float HistogramView::evaluateCurveAtAbscissa(void * curve, float t) const {
  return (float)m_data->sizeAtValue(t)/(float)m_data->totalSize();
}

}
