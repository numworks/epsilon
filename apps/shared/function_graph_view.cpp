#include "function_graph_view.h"
#include <assert.h>
#include <cmath>
#include <float.h>
using namespace Poincare;

namespace Shared {

FunctionGraphView::FunctionGraphView(InteractiveCurveViewRange * graphRange,
  CurveViewCursor * cursor, BannerView * bannerView, View * cursorView) :
  CurveView(graphRange, cursor, bannerView, cursorView),
  m_selectedFunction(nullptr),
  m_xLabels{},
  m_yLabels{},
  m_context(nullptr)
{
}

void FunctionGraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawAxes(ctx, rect, Axis::Vertical);
  drawLabels(ctx, rect, Axis::Horizontal, true);
  drawLabels(ctx, rect, Axis::Vertical, true);
}

void FunctionGraphView::setContext(Context * context) {
  m_context = context;
}

Context * FunctionGraphView::context() const {
  return m_context;
}

void FunctionGraphView::selectFunction(Function * function) {
  if (m_selectedFunction != function) {
    reload();
    m_selectedFunction = function;
    reload();
  }
}

char * FunctionGraphView::label(Axis axis, int index) const {
  return (axis == Axis::Horizontal ? (char *)m_xLabels[index] : (char *)m_yLabels[index]);
}

}
