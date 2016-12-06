#include "graph_view.h"
#include <assert.h>
#include <math.h>
#include <float.h>

namespace Graph {

constexpr KDColor GraphView::k_gridColor;

GraphView::GraphView(FunctionStore * functionStore, AxisInterval * axisInterval) :
  CurveView(),
  m_cursorView(CursorView()),
  m_xCursorPosition(-1.0f),
  m_yCursorPosition(-1.0f),
  m_visibleCursor(true),
  m_axisInterval(axisInterval),
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

float GraphView::scale(Axis axis) const {
  return (axis == Axis::Horizontal ? m_axisInterval->xScale() : m_axisInterval->yScale());
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

void GraphView::setXCursorPosition(float xPosition, Function * function) {
  float xRange = max(Axis::Horizontal) - min(Axis::Horizontal);
  float yRange = max(Axis::Horizontal) - min(Axis::Horizontal);
  m_axisInterval->setXMin(xPosition - xRange/2.0f);
  m_axisInterval->setXMax(xPosition + xRange/2.0f);
  m_xCursorPosition = floatToPixel(Axis::Horizontal, xPosition);
  float yPosition = function->evaluateAtAbscissa(xPosition, m_evaluateContext);
  m_axisInterval->setYAuto(false);
  m_axisInterval->setYMin(yPosition - yRange/2.0f);
  m_axisInterval->setYMax(yPosition + yRange/2.0f);
  m_yCursorPosition = floatToPixel(Axis::Vertical, yPosition);
  reload();
}

void GraphView::setVisibleCursor(bool visibleCursor) {
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
  bool outsideWindow = m_xCursorPosition < 0 || m_xCursorPosition > bounds().width() || m_yCursorPosition < 0 || m_yCursorPosition > bounds().height();
  m_xCursorPosition = m_xCursorPosition + xOffset;
  if (!outsideWindow && m_xCursorPosition < k_cursorMarginToBorder) {
    float xRange = max(Axis::Horizontal) - min(Axis::Horizontal);
    m_axisInterval->setXMin(pixelToFloat(Axis::Horizontal, floorf(m_xCursorPosition)-k_cursorMarginToBorder));
    m_axisInterval->setXMax(min(Axis::Horizontal) + xRange);
    m_xCursorPosition = m_xCursorPosition - floorf(m_xCursorPosition) + k_cursorMarginToBorder;
  }
  if (!outsideWindow && m_xCursorPosition > bounds().width() - k_cursorMarginToBorder) {
    float xRange = max(Axis::Horizontal) - min(Axis::Horizontal);
    m_axisInterval->setXMax(pixelToFloat(Axis::Horizontal, ceilf(m_xCursorPosition)+k_cursorMarginToBorder));
    m_axisInterval->setXMin(max(Axis::Horizontal) - xRange);
    m_xCursorPosition = bounds().width() - k_cursorMarginToBorder - ceilf(m_xCursorPosition) + m_xCursorPosition;
  }
  Function * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  float ordinate = f->evaluateAtAbscissa(pixelToFloat(Axis::Horizontal, m_xCursorPosition), m_evaluateContext);
  m_yCursorPosition = floatToPixel(Axis::Vertical, ordinate);
  if (!outsideWindow && m_yCursorPosition < k_cursorMarginToBorder) {
    float yRange = max(Axis::Vertical) - min(Axis::Vertical);
    m_axisInterval->setYMax(pixelToFloat(Axis::Vertical, floorf(m_yCursorPosition)-k_cursorMarginToBorder));
    m_axisInterval->setYMin(max(Axis::Vertical) - yRange);
    m_yCursorPosition = m_yCursorPosition - floorf(m_yCursorPosition) + k_cursorMarginToBorder;
  }
  if (!outsideWindow && m_yCursorPosition > bounds().height() - k_cursorMarginToBorder) {
    float yRange = max(Axis::Vertical) - min(Axis::Vertical);
    m_axisInterval->setYMin(pixelToFloat(Axis::Vertical, ceilf(m_yCursorPosition)+k_cursorMarginToBorder));
    m_axisInterval->setYMax(min(Axis::Vertical) + yRange);
    m_yCursorPosition = bounds().height() - k_cursorMarginToBorder - ceilf(m_yCursorPosition) + m_yCursorPosition;
  }
  reload();
}

Function * GraphView::moveCursorUp() {
  float x = pixelToFloat(Axis::Horizontal, m_xCursorPosition);
  Function * actualFunction = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  float y = actualFunction->evaluateAtAbscissa(x, m_evaluateContext);
  Function * nextFunction = actualFunction;
  float nextY = FLT_MAX;
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    Function * f = m_functionStore->activeFunctionAtIndex(i);
    float newY = f->evaluateAtAbscissa(x, m_evaluateContext);
    if (newY > y && newY < nextY) {
      m_indexFunctionSelectedByCursor = i;
      nextY = newY;
      nextFunction = f;
    }
  }
  if (nextFunction == actualFunction) {
    return nullptr;
  }
  m_yCursorPosition = floatToPixel(Axis::Vertical, nextY);
  layoutSubviews();
  return nextFunction;
}

Function * GraphView::moveCursorDown() {
  float x = pixelToFloat(Axis::Horizontal, m_xCursorPosition);
  Function * actualFunction = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  float y = actualFunction->evaluateAtAbscissa(x, m_evaluateContext);
  Function * nextFunction = actualFunction;
  float nextY = -FLT_MAX;
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    Function * f = m_functionStore->activeFunctionAtIndex(i);
    float newY = f->evaluateAtAbscissa(x, m_evaluateContext);
    if (newY < y && newY > nextY) {
      m_indexFunctionSelectedByCursor = i;
      nextY = newY;
      nextFunction = f;
    }
  }
  if (nextFunction == actualFunction) {
    return nullptr;
  }
  m_yCursorPosition = floatToPixel(Axis::Vertical, nextY);
  layoutSubviews();
  return nextFunction;
}

void GraphView::layoutSubviews() {
  KDRect cursorFrame((int)m_xCursorPosition - (k_cursorSize+1)/2+1, (int)m_yCursorPosition - (k_cursorSize+1)/2+1, k_cursorSize, k_cursorSize);
  if (!m_visibleCursor) {
    cursorFrame = KDRectZero;
  }
  m_cursorView.setPosition(m_xCursorPosition, m_yCursorPosition);
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
  drawGridLines(ctx, rect, Axis::Horizontal, m_axisInterval->xScale(), k_gridColor);
  drawGridLines(ctx, rect, Axis::Vertical, m_axisInterval->yScale(), k_gridColor);
}

float GraphView::min(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_axisInterval->xMin() : m_axisInterval->yMin());
}

float GraphView::max(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_axisInterval->xMax() : m_axisInterval->yMax());
}

float GraphView::evaluateExpressionAtAbscissa(Expression * expression, float abscissa) const {
  m_evaluateContext->setOverridenValueForSymbolX(abscissa);
  return expression->approximate(*m_evaluateContext);
}

}
