#include "graph_view.h"
#include <assert.h>
#include <math.h>
#include <float.h>

namespace Graph {

constexpr KDColor GraphView::k_gridColor;

GraphView::GraphView(FunctionStore * functionStore, GraphWindow * graphWindow) :
  CurveView(),
  m_cursorView(CursorView()),
  m_xCursorPosition(-1.0f),
  m_yCursorPosition(-1.0f),
  m_visibleCursor(true),
  m_graphWindow(graphWindow),
  m_functionStore(functionStore),
  m_context(nullptr)
{
}

BannerView * GraphView::bannerView() {
  return &m_bannerView;
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

void GraphView::setContext(Context * context) {
  m_context = context;
  m_bannerView.setContext(context);
}

Context * GraphView::context() const {
  return m_context;
}

int GraphView::indexFunctionSelectedByCursor() {
  return m_indexFunctionSelectedByCursor;
}

void GraphView::reload() {
  markRectAsDirty(bounds());
  layoutSubviews();
  computeLabels(Axis::Horizontal);
  computeLabels(Axis::Vertical);
}

void GraphView::reloadCursor() {
  markRectAsDirty(KDRect(KDPoint(roundf(m_xCursorPosition) - k_cursorSize/2, roundf(m_yCursorPosition)- k_cursorSize/2), k_cursorSize, k_cursorSize));
  layoutSubviews();
}

float GraphView::gridUnit(Axis axis) const {
  return (axis == Axis::Horizontal ? m_graphWindow->xGridUnit() : m_graphWindow->yGridUnit());
}

char * GraphView::label(Axis axis, int index) const {
  return (axis == Axis::Horizontal ? (char *)m_xLabels[index] : (char *)m_yLabels[index]);
}

float GraphView::xPixelCursorPosition() {
  return m_xCursorPosition;
}

float GraphView::xCursorPosition() {
  return min(Axis::Horizontal) + m_xCursorPosition*(max(Axis::Horizontal)-min(Axis::Horizontal))/pixelLength(Axis::Horizontal);
}

void GraphView::goToAbscissaOnFunction(float abscissa, Function * function) {
  m_graphWindow->centerAxisAround(GraphWindow::Axis::X, abscissa);
  m_xCursorPosition = floatToPixel(Axis::Horizontal, abscissa);
  float ordinate = function->evaluateAtAbscissa(abscissa, m_context);
  m_graphWindow->centerAxisAround(GraphWindow::Axis::Y, ordinate);
  m_yCursorPosition = floatToPixel(Axis::Vertical, ordinate);
  updateBannerView(function);
  reload();
}

void GraphView::setCursorVisible(bool visibleCursor) {
  m_visibleCursor = visibleCursor;
  layoutSubviews();
}

void GraphView::initCursorPosition() {
  float center = (min(Axis::Horizontal)+max(Axis::Horizontal))/2.0f;
  m_indexFunctionSelectedByCursor = 0;
  Function * firstFunction = m_functionStore->activeFunctionAtIndex(0);
  float fCenter = firstFunction->evaluateAtAbscissa(center, m_context);
  m_xCursorPosition = (bounds().width()-1.0f)/2.0f;
  m_yCursorPosition = floatToPixel(Axis::Vertical, fCenter);
  updateBannerView(firstFunction);
}

void GraphView::moveCursorHorizontally(KDCoordinate xOffset) {
  markRectAsDirty(KDRect(KDPoint(roundf(m_xCursorPosition) - k_cursorSize/2, roundf(m_yCursorPosition)- k_cursorSize/2), k_cursorSize, k_cursorSize));
  m_xCursorPosition = m_xCursorPosition + xOffset;
  float x = pixelToFloat(Axis::Horizontal, m_xCursorPosition);
  Function * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  float y = f->evaluateAtAbscissa(x, m_context);
  float xMargin = pixelToFloat(Axis::Horizontal, k_cursorMarginToBorder) - pixelToFloat(Axis::Horizontal, 0);
  float yMargin =  pixelToFloat(Axis::Vertical, 0) - pixelToFloat(Axis::Vertical, k_cursorMarginToBorder);
  bool windowHasMoved = m_graphWindow->panToMakePointVisible(x, y, xMargin, yMargin);
  m_xCursorPosition = floatToPixel(Axis::Horizontal, x);
  m_yCursorPosition = floatToPixel(Axis::Vertical, y);
  updateBannerView(f);
  if (windowHasMoved) {
    reload();
  } else {
    reloadCursor();
  }
}

Function * GraphView::moveCursorVertically(int direction) {
  float x = pixelToFloat(Axis::Horizontal, m_xCursorPosition);
  Function * actualFunction = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  float y = actualFunction->evaluateAtAbscissa(x, m_context);
  Function * nextFunction = actualFunction;
  float nextY = direction > 0 ? FLT_MAX : -FLT_MAX;
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    Function * f = m_functionStore->activeFunctionAtIndex(i);
    float newY = f->evaluateAtAbscissa(x, m_context);
    bool isNextFunction = direction > 0 ? (newY > y && newY < nextY) : (newY < y && newY > nextY);
    if (isNextFunction) {
      m_indexFunctionSelectedByCursor = i;
      nextY = newY;
      nextFunction = f;
    }
  }
  if (nextFunction == actualFunction) {
    return nullptr;
  }
  float xMargin = pixelToFloat(Axis::Horizontal, k_cursorMarginToBorder) - pixelToFloat(Axis::Horizontal, 0);
  float yMargin =  pixelToFloat(Axis::Vertical, 0) - pixelToFloat(Axis::Vertical, k_cursorMarginToBorder);
  bool windowHasMoved = m_graphWindow->panToMakePointVisible(x, nextY, xMargin, yMargin);
  markRectAsDirty(KDRect(KDPoint(roundf(m_xCursorPosition) - k_cursorSize/2, roundf(m_yCursorPosition)- k_cursorSize/2), k_cursorSize, k_cursorSize));
  m_xCursorPosition = floatToPixel(Axis::Horizontal, x);
  m_yCursorPosition = floatToPixel(Axis::Vertical, nextY);
  updateBannerView(nextFunction);
  if (windowHasMoved) {
    reload();
  } else {
    reloadCursor();
  }
  return nextFunction;
}

void GraphView::layoutSubviews() {
  KDRect cursorFrame(roundf(m_xCursorPosition) - k_cursorSize/2, roundf(m_yCursorPosition) - k_cursorSize/2, k_cursorSize, k_cursorSize);
  KDRect bannerFrame(KDRect(0, bounds().height()- k_bannerHeight, bounds().width(), k_bannerHeight));
  if (!m_visibleCursor) {
    cursorFrame = KDRectZero;
    bannerFrame = KDRectZero;
  }
  m_cursorView.setFrame(cursorFrame);
  m_bannerView.setFrame(bannerFrame);
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(Axis::Horizontal, ctx, rect);
  drawAxes(Axis::Vertical, ctx, rect);
  drawLabels(Axis::Horizontal, true, ctx, rect);
  drawLabels(Axis::Vertical, true, ctx, rect);
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    Function * f = m_functionStore->activeFunctionAtIndex(i);
    drawCurve(f, f->color(), ctx, rect);
  }
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

float GraphView::min(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_graphWindow->xMin() : m_graphWindow->yMin());
}

float GraphView::max(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_graphWindow->xMax() : m_graphWindow->yMax());
}

float GraphView::evaluateCurveAtAbscissa(void * curve, float abscissa) const {
  Function * f = (Function *)curve;
  return f->evaluateAtAbscissa(abscissa, m_context);
}

void GraphView::updateBannerView(Function * function) {
  m_bannerView.setAbscissa(xCursorPosition());
  m_bannerView.setFunction(function);
}

}
