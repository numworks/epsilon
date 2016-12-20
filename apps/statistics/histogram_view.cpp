#include "histogram_view.h"
#include <assert.h>

namespace Statistics {

HistogramView::HistogramView(Data * data) :
  CurveView(data),
  m_data(data),
  m_selectedBin(0.0f)
{
}

void HistogramView::reload() {
  markRectAsDirty(bounds());
  computeLabels(Axis::Horizontal);
  initSelectedBin();
}

void HistogramView::initSelectedBin() {
  m_selectedBin = m_data->xMin() + m_data->binWidth()/2;
  while (m_data->sizeAtValue(m_selectedBin) == 0) {
    m_selectedBin += m_data->binWidth();
  }
  markRectAsDirty(bounds());
}

void HistogramView::unselectBin() {
  m_selectedBin = m_data->xMin() - m_data->binWidth()/2;
  markRectAsDirty(bounds());
}

void HistogramView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(Axis::Horizontal, ctx, rect);
  drawAxes(Axis::Vertical, ctx, rect);
  drawLabels(Axis::Horizontal, true, ctx, rect);
  drawHistogram(m_data->binWidth(), KDColorBlack, ctx, rect, KDColorRed, m_selectedBin);
}

void HistogramView::selectNextBinToward(int direction) {
  float newSelectedBin = m_selectedBin;
  if (direction > 0.0f) {
    do {
      newSelectedBin += m_data->binWidth();
    } while (m_data->sizeAtValue(newSelectedBin) == 0 && newSelectedBin < m_data->xMax());
  }
  if (direction < 0.0f) {
    do {
      newSelectedBin -= m_data->binWidth();
    } while (m_data->sizeAtValue(newSelectedBin) == 0 && newSelectedBin > m_data->xMin());
  }
  if (newSelectedBin > m_data->xMin() && newSelectedBin < m_data->xMax()) {
    m_selectedBin = newSelectedBin;
    markRectAsDirty(bounds());
  }
}

float HistogramView::selectedBin() {
  return m_selectedBin;
}

char * HistogramView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

float HistogramView::evaluateCurveAtAbscissa(void * curve, float t) const {
  return m_data->sizeAtValue(t);
}

}
