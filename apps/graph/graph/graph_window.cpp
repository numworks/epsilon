#include "graph_window.h"
#include "../../constant.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <ion.h>

namespace Graph {

GraphWindow::GraphWindow(FunctionStore * functionStore) :
  m_xMin(-10.0f),
  m_xMax(10.0f),
  m_yMin(-10.0f),
  m_yMax(10.0f),
  m_yAuto(true),
  m_xGridUnit(2.0f),
  m_yGridUnit(2.0f),
  m_xCursorPosition(NAN),
  m_yCursorPosition(NAN),
  m_indexFunctionSelectedByCursor(0),
  m_functionStore(functionStore),
  m_context(nullptr)
{
}

float GraphWindow::xMin() {
  return m_xMin;
}

float GraphWindow::xMax() {
  return m_xMax;
}

float GraphWindow::yMin() {
  return m_yMin;
}

float GraphWindow::yMax() {
  return m_yMax;
}

bool GraphWindow::yAuto() {
  return m_yAuto;
}

float GraphWindow::xGridUnit() {
  return m_xGridUnit;
}

float GraphWindow::yGridUnit() {
  return m_yGridUnit;
}

void GraphWindow::setXMin(float xMin) {
  m_xMin = xMin;
  computeYaxes();
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
}

void GraphWindow::setXMax(float xMax) {
  m_xMax = xMax;
  computeYaxes();
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
}

void GraphWindow::setYMin(float yMin) {
  m_yMin = yMin;
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void GraphWindow::setYMax(float yMax) {
  m_yMax = yMax;
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void GraphWindow::setYAuto(bool yAuto) {
  m_yAuto = yAuto;
  computeYaxes();
}

bool GraphWindow::computeYaxes() {
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

void GraphWindow::zoom(float ratio) {
  float xMin = m_xMin;
  float xMax = m_xMax;
  float yMin = m_yMin;
  float yMax = m_yMax;
  m_xMin = (xMax+xMin)/2.0f - ratio*fabsf(xMax-xMin);
  m_xMax = (xMax+xMin)/2.0f + ratio*fabsf(xMax-xMin);
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yAuto = false;
  m_yMin = (yMax+yMin)/2.0f - ratio*fabsf(yMax-yMin);
  m_yMax = (yMax+yMin)/2.0f + ratio*fabsf(yMax-yMin);
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void GraphWindow::translateWindow(Direction direction) {
  m_yAuto = false;
  if (direction == Direction::Up) {
    m_yMin = m_yMin + m_yGridUnit;
    m_yMax = m_yMax + m_yGridUnit;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  }
  if (direction == Direction::Down) {
    m_yMin = m_yMin - m_yGridUnit;
    m_yMax = m_yMax - m_yGridUnit;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  }
  if (direction == Direction::Left) {
    m_xMin = m_xMin - m_xGridUnit;
    m_xMax = m_xMax - m_xGridUnit;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
    computeYaxes();
  }
  if (direction == Direction::Right) {
    m_xMin = m_xMin + m_xGridUnit;
    m_xMax = m_xMax + m_xGridUnit;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
    computeYaxes();
  }
}

void GraphWindow::setTrigonometric() {
  m_xMin = -10.5f;
  m_xMax = 10.5f;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yAuto = false;
  m_yMin = -1.6f;
  m_yMax = 1.6f;
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void GraphWindow::roundAbscissa() {
  float xMin = m_xMin;
  float xMax = m_xMax;
  m_xMin = roundf((xMin+xMax)/2) - 160.0f;
  m_xMax = roundf((xMin+xMax)/2) + 159.0f;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  computeYaxes();
}

void GraphWindow::normalize() {
  float xMin = m_xMin;
  float xMax = m_xMax;
  float yMin = m_yMin;
  float yMax = m_yMax;
  m_xMin = (xMin+xMax)/2 - 5.3f;
  m_xMax = (xMin+xMax)/2 + 5.3f;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yAuto = false;
  m_yMin = (yMin+yMax)/2 - 3.1f;
  m_yMax = (yMin+yMax)/2 + 3.1f;
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void GraphWindow::setDefault() {
  m_xMin = -10.0f;
  m_xMax = 10.0f;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  setYAuto(true);
}

float GraphWindow::xCursorPosition() {
  return m_xCursorPosition;
}

float GraphWindow::yCursorPosition() {
  return m_yCursorPosition;
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

bool GraphWindow::moveCursorHorizontally(int direction) {
  m_xCursorPosition = direction > 0 ? m_xCursorPosition + m_xGridUnit/k_numberOfCursorStepsInGradUnit :
    m_xCursorPosition - m_xGridUnit/k_numberOfCursorStepsInGradUnit;
  Function * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  m_yCursorPosition = f->evaluateAtAbscissa(m_xCursorPosition, m_context);
  float xMargin = k_cursorMarginFactorToBorder * (m_xMax - m_xMin);
  float yMargin = k_cursorMarginFactorToBorder * (m_yMax - m_yMin);
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
  float xMargin = k_cursorMarginFactorToBorder * (m_xMax - m_xMin);
  float yMargin = k_cursorMarginFactorToBorder * (m_yMax - m_yMin);
  m_yCursorPosition = nextY;
  bool windowHasMoved = panToMakePointVisible(m_xCursorPosition, m_yCursorPosition, xMargin, yMargin);
  return windowHasMoved;
}

bool GraphWindow::panToMakePointVisible(float x, float y, float xMargin, float yMargin) {
  bool windowMoved = false;
  float xRange = m_xMax - m_xMin;
  float yRange = m_yMax - m_yMin;
  if (x < m_xMin + xMargin) {
    m_xMin = x - xMargin;
    m_xMax = m_xMin + xRange;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
    computeYaxes();
    windowMoved = true;
  }
  if (x > m_xMax - xMargin) {
    m_xMax = x + xMargin;
    m_xMin = m_xMax - xRange;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
    computeYaxes();
    windowMoved = true;
  }
  if (y < m_yMin + yMargin) {
    m_yMin = y - yMargin;
    m_yMax = m_yMin + yRange;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
    computeYaxes();
    windowMoved = true;
  }
  if (y > m_yMax - yMargin) {
    m_yMax = y + yMargin;
    m_yMin = m_yMax - yRange;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
    computeYaxes();
    windowMoved = true;
  }
  return windowMoved;
}

void GraphWindow::centerAxisAround(Axis axis, float position) {
  if (axis == Axis::X) {
    float range = m_xMax - m_xMin;
    m_xMin = position - range/2.0f;
    m_xMax = position + range/2.0f;
    computeYaxes();
    m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  } else {
    m_yAuto = false;
    float range = m_yMax - m_yMin;
    m_yMin = position - range/2.0f;
    m_yMax = position + range/2.0f;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
  }
}

}
