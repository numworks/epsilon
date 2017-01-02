#include "graph_view.h"
#include <assert.h>
#include <math.h>

namespace Regression {

GraphView::GraphView(Data * data) :
  CurveViewWithBannerAndCursor(data, 0.05f, 0.05f, 0.25f, 0.05f),
  m_data(data),
  m_bannerView(BannerView(data))
{
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

BannerView * GraphView::bannerView() {
  return &m_bannerView;
}

float GraphView::evaluateModelWithParameter(Model * curve, float t) const {
  return m_data->yValueForXValue(t);
}

}
