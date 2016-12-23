#include "box_view.h"
#include <assert.h>
#include <math.h>

namespace Statistics {

BoxView::BoxView(Data * data) :
  CurveView(&m_boxWindow),
  m_data(data),
  m_boxWindow(BoxWindow(data)),
  m_selectedQuantile(-1)
{
}

void BoxView::reload(float dirtyVertical) {
  markRectAsDirty(bounds());
  computeLabels(Axis::Horizontal);
}

int BoxView::selectedQuantile() {
  return m_selectedQuantile;
}

void BoxView::selectQuantile(int selectedQuantile) {
  if (m_selectedQuantile != selectedQuantile) {
    m_selectedQuantile = selectedQuantile;
    markRectAsDirty(bounds());
  }
}

void BoxView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(Axis::Horizontal, ctx, rect);
  drawLabels(Axis::Horizontal, false, ctx, rect);
  drawSegment(ctx, rect, Axis::Vertical, m_data->minValue(), 0.4f, 0.6f, KDColorBlack);
  drawSegment(ctx, rect, Axis::Vertical, m_data->firstQuartile(), 0.2f, 0.8f, KDColorBlack);
  drawSegment(ctx, rect, Axis::Vertical, m_data->median(), 0.2f, 0.8f, KDColorBlack);
  drawSegment(ctx, rect, Axis::Vertical, m_data->thirdQuartile(), 0.2f, 0.8f, KDColorBlack);
  drawSegment(ctx, rect, Axis::Vertical, m_data->maxValue(), 0.4f, 0.6f, KDColorBlack);
  drawSegment(ctx, rect, Axis::Horizontal, 0.5f, m_data->minValue(), m_data->firstQuartile(), KDColorBlack);
  drawSegment(ctx, rect, Axis::Horizontal, 0.5f, m_data->thirdQuartile(), m_data->maxValue(), KDColorBlack);
  drawSegment(ctx, rect, Axis::Horizontal, 0.2f, m_data->firstQuartile(), m_data->thirdQuartile(), KDColorBlack);
  drawSegment(ctx, rect, Axis::Horizontal, 0.8f, m_data->firstQuartile(), m_data->thirdQuartile(), KDColorBlack);
}

char * BoxView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

float BoxView::evaluateCurveAtAbscissa(void * curve, float t) const {
  return 0.0f;
}

}
