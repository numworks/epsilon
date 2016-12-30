#include "graph_view.h"
#include <assert.h>
#include <math.h>

namespace Regression {

GraphView::GraphView(Data * data) :
  CurveViewWithBanner(data, 0.2f, 0.2f, 0.4f, 0.2f),
  m_data(data),
  m_bannerView(BannerView(data))
{
}

void GraphView::reloadSelection() {
  float pixelXSelection = roundf(floatToPixel(Axis::Horizontal, m_data->xCursorPosition()));
  float pixelYSelection = roundf(floatToPixel(Axis::Vertical, m_data->yCursorPosition()));
  KDRect dirtyZone(KDRect(pixelXSelection - k_dotSize/2, pixelYSelection - k_dotSize/2, k_dotSize, k_dotSize));
  markRectAsDirty(dirtyZone);
  m_bannerView.reload();
}

void GraphView::reload() {
  markRectAsDirty(bounds());
  computeLabels(Axis::Horizontal);
  computeLabels(Axis::Vertical);
  m_bannerView.reload();
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawAxes(ctx, rect, Axis::Vertical);
  drawLabels(ctx, rect, Axis::Horizontal, true);
  drawLabels(ctx, rect, Axis::Vertical, true);
  drawCurve(ctx, rect, nullptr, KDColorRed);
  for (int index = 0; index < m_data->numberOfPairs(); index++) {
    drawDot(ctx, rect, m_data->xValueAtIndex(index), m_data->yValueAtIndex(index), KDColorBlue, KDSize(k_dotSize, k_dotSize));
  }
}

char * GraphView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return (char *)m_yLabels[index];
  }
  return (char *)m_xLabels[index];
}

View * GraphView::bannerView() {
  return &m_bannerView;
}

float GraphView::evaluateModelWithParameter(Model * curve, float t) const {
  return m_data->yValueForXValue(t);
}

}
