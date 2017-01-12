#include "graph_view.h"
#include <assert.h>
#include <math.h>
#include <float.h>

namespace Graph {

GraphView::GraphView(FunctionStore * functionStore, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, ::BannerView * bannerView, View * cursorView) :
  CurveView(graphRange, cursor, bannerView, cursorView, 0.0f, 0.0f, 0.1f, 0.0f),
  m_functionStore(functionStore),
  m_context(nullptr)
{
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawAxes(ctx, rect, Axis::Vertical);
  drawLabels(ctx, rect, Axis::Horizontal, true);
  drawLabels(ctx, rect, Axis::Vertical, true);
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    Function * f = m_functionStore->activeFunctionAtIndex(i);
    drawCurve(ctx, rect, f, f->color());
  }
}

void GraphView::setContext(Context * context) {
  m_context = context;
}

Context * GraphView::context() const {
  return m_context;
}

char * GraphView::label(Axis axis, int index) const {
  return (axis == Axis::Horizontal ? (char *)m_xLabels[index] : (char *)m_yLabels[index]);
}

float GraphView::evaluateModelWithParameter(Model * curve, float abscissa) const {
  Function * f = (Function *)curve;
  return f->evaluateAtAbscissa(abscissa, m_context);
}

}
