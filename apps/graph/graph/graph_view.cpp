#include "graph_view.h"

using namespace Shared;

namespace Graph {

GraphView::GraphView(CartesianFunctionStore * functionStore, InteractiveCurveViewRange * graphRange,
  CurveViewCursor * cursor, BannerView * bannerView, View * cursorView) :
  FunctionGraphView(graphRange, cursor, bannerView, cursorView),
  m_functionStore(functionStore),
  m_type(Type::Default)
{
}

void GraphView::reload() {
  if (m_type == Type::Tangent) {
    KDRect dirtyZone(KDRect(0, 0, bounds().width(), bounds().height()-m_bannerView->bounds().height()));
    markRectAsDirty(dirtyZone);
  }
  return FunctionGraphView::reload();
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  FunctionGraphView::drawRect(ctx, rect);
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    CartesianFunction * f = m_functionStore->activeFunctionAtIndex(i);
    drawCurve(ctx, rect, [](float t, void * model, void * context) {
        CartesianFunction * f = (CartesianFunction *)model;
        Poincare::Context * c = (Poincare::Context *)context;
        return f->evaluateAtAbscissa(t, c);
      }, f, context(), f->color());
    if (m_type == Type::Tangent && f == m_selectedFunction) {
      float tangentParameter[2];
      tangentParameter[0] = f->approximateDerivative(m_curveViewCursor->x(), context());
      tangentParameter[1] = -tangentParameter[0]*m_curveViewCursor->x()+f->evaluateAtAbscissa(m_curveViewCursor->x(), context());
      drawCurve(ctx, rect, [](float t, void * model, void * context) {
          float * tangent = (float *)model;
          return tangent[0]*t+tangent[1];
        }, tangentParameter, nullptr, KDColorBlack);
    }
  }
}

}
