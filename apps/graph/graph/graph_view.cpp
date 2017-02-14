#include "graph_view.h"
#include <assert.h>
#include <math.h>
#include <float.h>
using namespace Poincare;
using namespace Shared;

namespace Graph {

GraphView::GraphView(CartesianFunctionStore * functionStore, InteractiveCurveViewRange * graphRange,
  CurveViewCursor * cursor, BannerView * bannerView, View * cursorView) :
  CurveView(graphRange, cursor, bannerView, cursorView),
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
    CartesianFunction * f = m_functionStore->activeFunctionAtIndex(i);
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
  CartesianFunction * f = (CartesianFunction *)curve;
  return f->evaluateAtAbscissa(abscissa, m_context);
}

}
