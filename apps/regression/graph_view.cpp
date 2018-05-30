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
  for (int series = 0; series < Store::k_numberOfSeries; series++) {
    if (!m_store->seriesIsEmpty(series)) {
      KDColor color = Palette::DataColor[series];
      float regressionParameters[2] = {(float)m_store->slope(series), (float)m_store->yIntercept(series)};
      drawCurve(ctx, rect, [](float abscissa, void * model, void * context) {
          float * params = (float *)model;
          return params[0]*abscissa+params[1];
          },
          regressionParameters, nullptr, color);
      for (int index = 0; index < m_store->numberOfPairsOfSeries(series); index++) {
        drawDot(ctx, rect, m_store->get(series, 0, index), m_store->get(series, 1, index), color);
      }
      drawDot(ctx, rect, m_store->meanOfColumn(series, 0), m_store->meanOfColumn(series, 1), color, true);
      drawDot(ctx, rect, m_store->meanOfColumn(series, 0), m_store->meanOfColumn(series, 1), KDColorWhite);
    }
  }
}

char * GraphView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return (char *)m_yLabels[index];
  }
  return (char *)m_xLabels[index];
}

}
