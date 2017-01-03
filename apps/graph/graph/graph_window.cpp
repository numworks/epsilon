#include "graph_window.h"
#include "../../constant.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <ion.h>

namespace Graph {

GraphWindow::GraphWindow(FunctionStore * functionStore) :
  CurveViewWindowWithCursor(),
  m_indexFunctionSelectedByCursor(0),
  m_functionStore(functionStore),
  m_context(nullptr)
{
}

bool GraphWindow::computeYaxis() {
  if (!m_yAuto) {
    return false;
  }
  if (m_functionStore->numberOfActiveFunctions() > 0) {
    float min = FLT_MAX;
    float max = -FLT_MAX;
    float step = (m_xMax - m_xMin)/Ion::Display::Width;
    for (int i=0; i<m_functionStore->numberOfActiveFunctions(); i++) {
      Function * f = m_functionStore->activeFunctionAtIndex(i);
      float y = 0.0f;
      for (float x = m_xMin; x <= m_xMax; x += step) {
        y = f->evaluateAtAbscissa(x, m_context);
        if (!isnan(y) && !isinf(y)) {
          min = min < y ? min : y;
          max = max > y ? max : y;
        }
      }
    }
    if (m_yMin == min && m_yMax == max) {
      return false;
    }
    m_yMin = min;
    m_yMax = max;
    if (m_yMin == m_yMax) {
      m_yMin = min - 1;
      m_yMax = max + 1;
    }
  }
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  return true;
}

Context * GraphWindow::context() {
  return m_context;
}

void GraphWindow::setContext(Context * context) {
  m_context = context;
}

void GraphWindow::setTrigonometric() {
  m_xMin = -10.5f;
  m_xMax = 10.5f;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yAuto = false;
  m_yMin = -1.6f;
  m_yMax = 1.6f;
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  initCursorPosition();
}

void GraphWindow::setDefault() {
  m_xMin = -10.0f;
  m_xMax = 10.0f;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  setYAuto(true);
  initCursorPosition();
}

float GraphWindow::derivativeAtCursorPosition() {
  Function * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  return f->approximateDerivative(m_xCursorPosition, m_context);
}

Function * GraphWindow::functionSelectedByCursor() {
  return m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
}

void GraphWindow::setCursorPositionAtAbscissaWithFunction(float abscissa, Function * function) {
  m_xCursorPosition = abscissa;
  centerAxisAround(GraphWindow::Axis::X, m_xCursorPosition);
  m_yCursorPosition = function->evaluateAtAbscissa(m_xCursorPosition, m_context);
  centerAxisAround(GraphWindow::Axis::Y, m_yCursorPosition);
}

void GraphWindow::initCursorPosition() {
  m_xCursorPosition = (m_xMin+m_xMax)/2.0f;
  m_indexFunctionSelectedByCursor = 0;
  Function * firstFunction = m_functionStore->activeFunctionAtIndex(0);
  m_yCursorPosition = firstFunction->evaluateAtAbscissa(m_xCursorPosition, m_context);
}

int GraphWindow::moveCursorHorizontally(int direction) {
  m_xCursorPosition = direction > 0 ? m_xCursorPosition + m_xGridUnit/CurveViewWindowWithCursor::k_numberOfCursorStepsInGradUnit :
    m_xCursorPosition - m_xGridUnit/CurveViewWindowWithCursor::k_numberOfCursorStepsInGradUnit;
  Function * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  m_yCursorPosition = f->evaluateAtAbscissa(m_xCursorPosition, m_context);
  float xMargin = CurveViewWindowWithCursor::k_cursorMarginFactorToBorder * (m_xMax - m_xMin);
  float yMargin = CurveViewWindowWithCursor::k_cursorMarginFactorToBorder * (m_yMax - m_yMin);
  bool windowHasMoved = panToMakePointVisible(m_xCursorPosition, m_yCursorPosition, xMargin, yMargin);
  return windowHasMoved;
}

int GraphWindow::moveCursorVertically(int direction) {
  Function * actualFunction = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  float y = actualFunction->evaluateAtAbscissa(m_xCursorPosition, m_context);
  Function * nextFunction = actualFunction;
  float nextY = direction > 0 ? FLT_MAX : -FLT_MAX;
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    Function * f = m_functionStore->activeFunctionAtIndex(i);
    float newY = f->evaluateAtAbscissa(m_xCursorPosition, m_context);
    bool isNextFunction = direction > 0 ? (newY > y && newY < nextY) : (newY < y && newY > nextY);
    if (isNextFunction) {
      m_indexFunctionSelectedByCursor = i;
      nextY = newY;
      nextFunction = f;
    }
  }
  if (nextFunction == actualFunction) {
    return -1;
  }
  float xMargin = CurveViewWindowWithCursor::k_cursorMarginFactorToBorder * (m_xMax - m_xMin);
  float yMargin = CurveViewWindowWithCursor::k_cursorMarginFactorToBorder * (m_yMax - m_yMin);
  m_yCursorPosition = nextY;
  bool windowHasMoved = panToMakePointVisible(m_xCursorPosition, m_yCursorPosition, xMargin, yMargin);
  return windowHasMoved;
}

}
