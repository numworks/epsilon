#include "histogram_view.h"
#include <assert.h>

namespace Statistics {

HistogramView::HistogramView(Data * data) :
  CurveView(data),
  m_data(data)
{
}

void HistogramView::reload() {
  markRectAsDirty(bounds());
  computeLabels(Axis::Horizontal);
}

void HistogramView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(Axis::Horizontal, ctx, rect);
  drawAxes(Axis::Vertical, ctx, rect);
  drawLabels(Axis::Horizontal, true, ctx, rect);
  drawHistogram(nullptr, KDColorBlack, ctx, rect);
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
