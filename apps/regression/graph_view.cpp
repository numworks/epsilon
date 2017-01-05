#include "graph_view.h"
#include <assert.h>
#include <math.h>

namespace Regression {

GraphView::GraphView(Store * store) :
  CurveViewWithBannerAndCursor(store, 0.05f, 0.05f, 0.25f, 0.05f),
  m_store(store),
  m_bannerView(BannerView(store))
{
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawAxes(ctx, rect, Axis::Vertical);
  drawLabels(ctx, rect, Axis::Horizontal, true);
  drawLabels(ctx, rect, Axis::Vertical, true);
  drawCurve(ctx, rect, nullptr, KDColorRed);
  for (int index = 0; index < m_store->numberOfPairs(); index++) {
    drawDot(ctx, rect, m_store->get(0,index), m_store->get(1,index), KDColorBlue, KDSize(k_dotSize, k_dotSize));
  }
  drawDot(ctx, rect, m_store->meanOfColumn(0), m_store->meanOfColumn(1), KDColorGreen, KDSize(k_dotSize, k_dotSize));
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
  return m_store->yValueForXValue(t);
}

}
