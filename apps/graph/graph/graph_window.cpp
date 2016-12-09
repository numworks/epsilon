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
  m_xScale(2.0f),
  m_yScale(2.0f),
  m_functionStore(functionStore),
  m_evaluateContext(nullptr)
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

float GraphWindow::xScale() {
  return m_xScale;
}

float GraphWindow::yScale() {
  return m_yScale;
}

void GraphWindow::setXMin(float xMin) {
  m_xMin = xMin;
  computeYaxes();
  computeXScale();
}

void GraphWindow::setXMax(float xMax) {
  m_xMax = xMax;
  computeYaxes();
  computeXScale();
}

void GraphWindow::setYMin(float yMin) {
  m_yMin = yMin;
  computeYScale();
}

void GraphWindow::setYMax(float yMax) {
  m_yMax = yMax;
  computeYScale();
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
        y = f->evaluateAtAbscissa(x, m_evaluateContext);
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
  computeYScale();
  return true;
}

Context * GraphWindow::context() {
  return m_evaluateContext;
}

void GraphWindow::setContext(Context * context) {
  m_evaluateContext = (EvaluateContext *)context;
}

void GraphWindow::zoom(float ratio) {
  float xMin = m_xMin;
  float xMax = m_xMax;
  float yMin = m_yMin;
  float yMax = m_yMax;
  m_xMin = (xMax+xMin)/2.0f - ratio*fabsf(xMax-xMin);
  m_xMax = (xMax+xMin)/2.0f + ratio*fabsf(xMax-xMin);
  computeXScale();
  m_yAuto = false;
  m_yMin = (yMax+yMin)/2.0f - ratio*fabsf(yMax-yMin);
  m_yMax = (yMax+yMin)/2.0f + ratio*fabsf(yMax-yMin);
  computeYScale();
}

void GraphWindow::centerAxisAround(Axis axis, float position) {
  if (axis == Axis::X) {
    float range = m_xMax - m_xMin;
    m_xMin = position - range/2.0f;
    m_xMax = position + range/2.0f;
    computeYaxes();
    computeXScale();
  } else {
    m_yAuto = false;
    float range = m_yMax - m_yMin;
    m_yMin = position - range/2.0f;
    m_yMax = position + range/2.0f;
    computeYScale();
  }
}

void GraphWindow::translateWindow(Direction direction) {
  m_yAuto = false;
  if (direction == Direction::Up) {
    m_yMin = m_yMin + m_yScale;
    m_yMax = m_yMax + m_yScale;
    computeYScale();
  }
  if (direction == Direction::Down) {
    m_yMin = m_yMin - m_yScale;
    m_yMax = m_yMax - m_yScale;
    computeYScale();
  }
  if (direction == Direction::Left) {
    m_xMin = m_xMin - m_xScale;
    m_xMax = m_xMax - m_xScale;
    computeXScale();
    computeYaxes();
  }
  if (direction == Direction::Right) {
    m_xMin = m_xMin + m_xScale;
    m_xMax = m_xMax + m_xScale;
    computeXScale();
    computeYaxes();
  }
}

void GraphWindow::setTrigonometric() {
  m_xMin = -10.5f;
  m_xMax = 10.5f;
  computeXScale();
  m_yAuto = false;
  m_yMin = -1.6f;
  m_yMax = 1.6f;
  computeYScale();
}

void GraphWindow::roundAbscissa() {
  float xMin = m_xMin;
  float xMax = m_xMax;
  m_xMin = roundf((xMin+xMax)/2) - 160.0f;
  m_xMax = roundf((xMin+xMax)/2) + 159.0f;
  computeXScale();
  computeYaxes();
}

void GraphWindow::normalize() {
  float xMin = m_xMin;
  float xMax = m_xMax;
  float yMin = m_yMin;
  float yMax = m_yMax;
  m_xMin = (xMin+xMax)/2 - 5.3f;
  m_xMax = (xMin+xMax)/2 + 5.3f;
  computeXScale();
  m_yAuto = false;
  m_yMin = (yMin+yMax)/2 - 3.1f;
  m_yMax = (yMin+yMax)/2 + 3.1f;
  computeYScale();
}

void GraphWindow::setDefault() {
  m_xMin = -10.0f;
  m_xMax = 10.0f;
  computeXScale();
  setYAuto(true);
}

void GraphWindow::panToMakePointVisible(float x, float y, float xMargin, float yMargin) {
  float xRange = m_xMax - m_xMin;
  float yRange = m_yMax - m_yMin;
  if (x < m_xMin + xMargin) {
    m_xMin = x - xMargin;
    m_xMax = m_xMin + xRange;
    computeXScale();
    computeYaxes();
  }
  if (x > m_xMax - xMargin) {
    m_xMax = x + xMargin;
    m_xMin = m_xMax - xRange;
    computeXScale();
    computeYaxes();
  }
  if (y < m_yMin + yMargin) {
    m_yMin = y - yMargin;
    m_yMax = m_yMin + yRange;
    computeYScale();
  }
  if (y > m_yMax - yMargin) {
    m_yMax = y + yMargin;
    m_yMin = m_yMax - yRange;
    computeYScale();
  }
}

void GraphWindow::computeXScale() {
  int a = 0;
  int b = 0;
  float d = m_xMax - m_xMin;
  if (floorf(log10f(d/90.0f)) != floorf(log10f(d/35.0f))) {
    b = floorf(log10f(d/35.0f));
    a = 5;
  }
  if (floorf(log10f(d/36.0f)) != floorf(log10f(d/14.0f))) {
    b = floorf(log10f(d/14.0f));
    a = 2;
  }
  if (floorf(log10f(d/18.0f)) != floorf(log10f(d/7.0f))) {
    b = floorf(log10f(d/7.0f));
    a = 1;
  }
  m_xScale = a*powf(10,b);
}

void GraphWindow::computeYScale() {
  int a = 0;
  int b = 0;
  float d = m_yMax - m_yMin;
  if (floorf(log10f(d/65.0f)) != floorf(log10f(d/25.0f))) {
    b = floorf(log10f(d/25.0f));
    a = 5;
  }
  if (floorf(log10f(d/26.0f)) != floorf(log10f(d/10.0f))) {
    b = floorf(log10f(d/10.0f));
    a = 2;
  }
  if (floorf(log10f(d/13.0f)) != floorf(log10f(d/5.0f))) {
    b = floorf(log10f(d/5.0f));
    a = 1;
  }
  m_yScale = a*powf(10,b);
}

}
