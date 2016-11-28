#include "axis_interval.h"
#include "../../constant.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <ion.h>

namespace Graph {

AxisInterval::AxisInterval(FunctionStore * functionStore) :
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

float AxisInterval::xMin() {
  return m_xMin;
}

float AxisInterval::xMax() {
  return m_xMax;
}

float AxisInterval::yMin() {
  return m_yMin;
}

float AxisInterval::yMax() {
  return m_yMax;
}

bool AxisInterval::yAuto() {
  return m_yAuto;
}

float AxisInterval::xScale() {
  return m_xScale;
}

float AxisInterval::yScale() {
  return m_yScale;
}

void AxisInterval::setXMin(float xMin) {
  m_xMin = xMin;
  computeYaxes();
  computeXScale();
}

void AxisInterval::setXMax(float xMax) {
  m_xMax = xMax;
  computeYaxes();
  computeXScale();
}

void AxisInterval::setYMin(float yMin) {
  m_yMin = yMin;
  computeYScale();
}

void AxisInterval::setYMax(float yMax) {
  m_yMax = yMax;
  computeYScale();
}

void AxisInterval::setYAuto(bool yAuto) {
  m_yAuto = yAuto;
  computeYaxes();
}

void AxisInterval::computeYaxes() {
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

void AxisInterval::computeXScale() {
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

void AxisInterval::computeYScale() {
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

Context * AxisInterval::context() {
  return m_evaluateContext;
}

void AxisInterval::setContext(Context * context) {
  m_evaluateContext = (EvaluateContext *)context;
}

}
