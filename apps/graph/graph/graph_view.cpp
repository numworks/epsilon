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
  m_evaluateContext(nullptr)
{
}

int GraphView::numberOfSubviews() const {
  return 1;
};

View * GraphView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_cursorView;
}

void GraphView::setContext(Context * context) {
  m_evaluateContext = (EvaluateContext *)context;
}

Context * GraphView::context() const {
  return m_evaluateContext;
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
  float ordinate = function->evaluateAtAbscissa(abscissa, m_evaluateContext);
  m_graphWindow->centerAxisAround(GraphWindow::Axis::Y, ordinate);
  m_yCursorPosition = floatToPixel(Axis::Vertical, ordinate);
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
  float fCenter = firstFunction->evaluateAtAbscissa(center, m_evaluateContext);
  m_xCursorPosition = (bounds().width()-1.0f)/2.0f;
  m_yCursorPosition = floatToPixel(Axis::Vertical, fCenter);
}

void GraphView::moveCursorHorizontally(KDCoordinate xOffset) {
  m_xCursorPosition = m_xCursorPosition + xOffset;
  float x = pixelToFloat(Axis::Horizontal, m_xCursorPosition);
  Function * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  float y = f->evaluateAtAbscissa(x, m_evaluateContext);
  float xMargin = pixelToFloat(Axis::Horizontal, k_cursorMarginToBorder) - pixelToFloat(Axis::Horizontal, 0);
  float yMargin =  pixelToFloat(Axis::Vertical, 0) - pixelToFloat(Axis::Vertical, k_cursorMarginToBorder);
  m_graphWindow->panToMakePointVisible(x, y, xMargin, yMargin);
  m_xCursorPosition = floatToPixel(Axis::Horizontal, x);
  m_yCursorPosition = floatToPixel(Axis::Vertical, y);
  reload();
}

Function * GraphView::moveCursorVertically(int direction) {
  float x = pixelToFloat(Axis::Horizontal, m_xCursorPosition);
  Function * actualFunction = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  float y = actualFunction->evaluateAtAbscissa(x, m_evaluateContext);
  Function * nextFunction = actualFunction;
  float nextY = direction > 0 ? FLT_MAX : -FLT_MAX;
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    Function * f = m_functionStore->activeFunctionAtIndex(i);
    float newY = f->evaluateAtAbscissa(x, m_evaluateContext);
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
  m_graphWindow->panToMakePointVisible(x, nextY, xMargin, yMargin);
  m_xCursorPosition = floatToPixel(Axis::Horizontal, x);
  m_yCursorPosition = floatToPixel(Axis::Vertical, nextY);
  reload();
  return nextFunction;
}

void GraphView::layoutSubviews() {
  KDRect cursorFrame(roundf(m_xCursorPosition) - k_cursorSize/2, roundf(m_yCursorPosition) - k_cursorSize/2, k_cursorSize, k_cursorSize);
  if (!m_visibleCursor) {
    cursorFrame = KDRectZero;
  }
  m_cursorView.setFrame(cursorFrame);
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(Axis::Horizontal, ctx, rect);
  drawAxes(Axis::Vertical, ctx, rect);
  drawLabels(Axis::Horizontal, ctx, rect);
  drawLabels(Axis::Vertical, ctx, rect);
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    Function * f = m_functionStore->activeFunctionAtIndex(i);
    drawExpression(f->expression(), f->color(), ctx, rect);
  }
}

void GraphView::drawGridLines(KDContext * ctx, KDRect rect, Axis axis, float step, KDColor color) const {
  float start = step*((int)(min(axis)/step));
  Axis otherAxis = (axis == Axis::Horizontal) ? Axis::Vertical : Axis::Horizontal;
  for (float x =start; x < max(axis); x += step) {
    drawLine(ctx, rect, otherAxis, x, color);
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

float GraphView::evaluateExpressionAtAbscissa(Expression * expression, float abscissa) const {
  m_evaluateContext->setOverridenValueForSymbolX(abscissa);
  return expression->approximate(*m_evaluateContext);
}

}
