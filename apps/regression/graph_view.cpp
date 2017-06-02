#include "graph_view.h"
#include <assert.h>
#include <math.h>

using namespace Shared;

namespace Regression {

GraphView::GraphView(Store * store, CurveViewCursor * cursor, BannerView * bannerView, View * cursorView) :
  CurveView(store, cursor, bannerView, cursorView),
  m_store(store),
  m_xLabels{},
  m_yLabels{}
{
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawAxes(ctx, rect, Axis::Vertical);
  drawLabels(ctx, rect, Axis::Horizontal, true);
  drawLabels(ctx, rect, Axis::Vertical, true);
  drawCurve(ctx, rect, nullptr, Palette::YellowDark);
  for (int index = 0; index < m_store->numberOfPairs(); index++) {
    drawDot(ctx, rect, m_store->get(0,index), m_store->get(1,index), Palette::Red);
  }
  drawDot(ctx, rect, m_store->meanOfColumn(0), m_store->meanOfColumn(1), Palette::Red, true);
}

char * GraphView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return (char *)m_yLabels[index];
  }
  return (char *)m_xLabels[index];
}

float GraphView::evaluateModelWithParameter(Model * curve, float t) const {
  return m_store->yValueForXValue(t);
}

}
