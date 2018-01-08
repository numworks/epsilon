#include "graph_view.h"
#include <assert.h>

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
  float regressionParameters[2] = {(float)m_store->slope(), (float)m_store->yIntercept()};
  drawCurve(ctx, rect, [](float abscissa, void * model, void * context) {
        float * params = (float *)model;
        return params[0]*abscissa+params[1];
      },
    regressionParameters, nullptr, Palette::YellowDark);
  for (int index = 0; index < m_store->numberOfPairs(); index++) {
    drawDot(ctx, rect, m_store->get(0,index), m_store->get(1,index), Palette::Red);
  }
  drawDot(ctx, rect, m_store->meanOfColumn(0), m_store->meanOfColumn(1), Palette::Palette::YellowDark, true);
  drawDot(ctx, rect, m_store->meanOfColumn(0), m_store->meanOfColumn(1), KDColorWhite);
}

char * GraphView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return (char *)m_yLabels[index];
  }
  return (char *)m_xLabels[index];
}

}
