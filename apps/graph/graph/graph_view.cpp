#include "graph_view.h"
#include <assert.h>
#include <math.h>
#include <float.h>

namespace Graph {

constexpr KDColor GraphView::k_gridColor;

GraphView::GraphView(FunctionStore * functionStore, GraphWindow * graphWindow) :
  CurveViewWithBanner(graphWindow, 0.0f, 0.0f, 0.2f, 0.0f),
  m_bannerView(BannerView(graphWindow)),
  m_cursorView(CursorView()),
  m_graphWindow(graphWindow),
  m_functionStore(functionStore),
  m_context(nullptr)
{
}

View * GraphView::bannerView() {
  return &m_bannerView;
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

void GraphView::reload() {
  markRectAsDirty(bounds());
  layoutSubviews();
  computeLabels(Axis::Horizontal);
  computeLabels(Axis::Vertical);
  m_bannerView.reload();
}

void GraphView::reloadSelection() {
  KDCoordinate xCursorPixelPosition = roundf(floatToPixel(Axis::Horizontal, m_graphWindow->xCursorPosition()));
  KDCoordinate yCursorPixelPosition = roundf(floatToPixel(Axis::Vertical, m_graphWindow->yCursorPosition()));
  markRectAsDirty(KDRect(KDPoint(xCursorPixelPosition- k_cursorSize/2, yCursorPixelPosition - k_cursorSize/2), k_cursorSize, k_cursorSize));
  layoutSubviews();
  m_bannerView.reload();
}

void GraphView::layoutSubviews() {
  KDCoordinate xCursorPixelPosition = roundf(floatToPixel(Axis::Horizontal, m_graphWindow->xCursorPosition()));
  KDCoordinate yCursorPixelPosition = roundf(floatToPixel(Axis::Vertical, m_graphWindow->yCursorPosition()));
  KDRect cursorFrame(xCursorPixelPosition - k_cursorSize/2, yCursorPixelPosition - k_cursorSize/2, k_cursorSize, k_cursorSize);
  if (!m_mainViewSelected) {
    cursorFrame = KDRectZero;
  }
  m_cursorView.setFrame(cursorFrame);
  CurveViewWithBanner::layoutSubviews();
}

int GraphView::numberOfSubviews() const {
  return 2;
};

View * GraphView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &m_cursorView;
  }
  return &m_bannerView;
}

char * GraphView::label(Axis axis, int index) const {
  return (axis == Axis::Horizontal ? (char *)m_xLabels[index] : (char *)m_yLabels[index]);
}

void GraphView::drawGridLines(KDContext * ctx, KDRect rect, Axis axis, float step, KDColor color) const {
  float rectMin = pixelToFloat(Axis::Horizontal, rect.left());
  float rectMax = pixelToFloat(Axis::Horizontal, rect.right());
  if (axis == Axis::Vertical) {
    rectMax = pixelToFloat(Axis::Vertical, rect.top());
    rectMin = pixelToFloat(Axis::Vertical, rect.bottom());
  }
  float start = step*((int)(min(axis)/step));
  Axis otherAxis = (axis == Axis::Horizontal) ? Axis::Vertical : Axis::Horizontal;
  for (float x =start; x < max(axis); x += step) {
    if (rectMin <= x && x <= rectMax) {
      drawLine(ctx, rect, otherAxis, x, color);
    }
  }
}

void GraphView::drawGrid(KDContext * ctx, KDRect rect) const {
  drawGridLines(ctx, rect, Axis::Horizontal, m_graphWindow->xGridUnit(), k_gridColor);
  drawGridLines(ctx, rect, Axis::Vertical, m_graphWindow->yGridUnit(), k_gridColor);
}

float GraphView::evaluateModelWithParameter(Model * curve, float abscissa) const {
  Function * f = (Function *)curve;
  return f->evaluateAtAbscissa(abscissa, m_context);
}

}
