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

void GraphWindow::computeYaxes() {
  if (!m_yAuto) {
    return;
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
    m_yMin = min;
    m_yMax = max;
    if (m_yMin == m_yMax) {
      m_yMin = min - 1;
      m_yMax = max + 1;
    }
  }
  computeYScale();
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

Context * GraphWindow::context() {
  return m_evaluateContext;
}

void GraphWindow::setContext(Context * context) {
  m_evaluateContext = (EvaluateContext *)context;
}

}
