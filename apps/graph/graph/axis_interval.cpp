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

void AxisInterval::setXMin(float xMin) {
  m_xMin = xMin;
  computeYaxes();
}

void AxisInterval::setXMax(float xMax) {
  m_xMax = xMax;
  computeYaxes();
}

void AxisInterval::setYMin(float yMin) {
  m_yMin = yMin;
}

void AxisInterval::setYMax(float yMax) {
  m_yMax = yMax;
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
  }
}

Context * AxisInterval::context() {
  return m_evaluateContext;
}

void AxisInterval::setContext(Context * context) {
  m_evaluateContext = (EvaluateContext *)context;
}

}
