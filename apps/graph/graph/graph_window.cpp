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
  m_xGridUnit = computeGridUnit(Axis::X);
}

void GraphWindow::setXMax(float xMax) {
  m_xMax = xMax;
  computeYaxes();
  m_xGridUnit = computeGridUnit(Axis::X);
}

void GraphWindow::setYMin(float yMin) {
  m_yMin = yMin;
  m_yGridUnit = computeGridUnit(Axis::Y);
}

void GraphWindow::setYMax(float yMax) {
  m_yMax = yMax;
  m_yGridUnit = computeGridUnit(Axis::Y);
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
  m_yGridUnit = computeGridUnit(Axis::Y);
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
  m_xGridUnit = computeGridUnit(Axis::X);
  m_yAuto = false;
  m_yMin = (yMax+yMin)/2.0f - ratio*fabsf(yMax-yMin);
  m_yMax = (yMax+yMin)/2.0f + ratio*fabsf(yMax-yMin);
  m_yGridUnit = computeGridUnit(Axis::Y);
}

void GraphWindow::centerAxisAround(Axis axis, float position) {
  if (axis == Axis::X) {
    float range = m_xMax - m_xMin;
    m_xMin = position - range/2.0f;
    m_xMax = position + range/2.0f;
    computeYaxes();
    m_xGridUnit = computeGridUnit(Axis::X);
  } else {
    m_yAuto = false;
    float range = m_yMax - m_yMin;
    m_yMin = position - range/2.0f;
    m_yMax = position + range/2.0f;
    m_yGridUnit = computeGridUnit(Axis::Y);
  }
}

void GraphWindow::translateWindow(Direction direction) {
  m_yAuto = false;
  if (direction == Direction::Up) {
    m_yMin = m_yMin + m_yGridUnit;
    m_yMax = m_yMax + m_yGridUnit;
    m_yGridUnit = computeGridUnit(Axis::Y);
  }
  if (direction == Direction::Down) {
    m_yMin = m_yMin - m_yGridUnit;
    m_yMax = m_yMax - m_yGridUnit;
    m_yGridUnit = computeGridUnit(Axis::Y);
  }
  if (direction == Direction::Left) {
    m_xMin = m_xMin - m_xGridUnit;
    m_xMax = m_xMax - m_xGridUnit;
    m_xGridUnit = computeGridUnit(Axis::X);
    computeYaxes();
  }
  if (direction == Direction::Right) {
    m_xMin = m_xMin + m_xGridUnit;
    m_xMax = m_xMax + m_xGridUnit;
    m_xGridUnit = computeGridUnit(Axis::X);
    computeYaxes();
  }
}

void GraphWindow::setTrigonometric() {
  m_xMin = -10.5f;
  m_xMax = 10.5f;
  m_xGridUnit = computeGridUnit(Axis::X);
  m_yAuto = false;
  m_yMin = -1.6f;
  m_yMax = 1.6f;
  m_yGridUnit = computeGridUnit(Axis::Y);
}

void GraphWindow::roundAbscissa() {
  float xMin = m_xMin;
  float xMax = m_xMax;
  m_xMin = roundf((xMin+xMax)/2) - 160.0f;
  m_xMax = roundf((xMin+xMax)/2) + 159.0f;
  m_xGridUnit = computeGridUnit(Axis::X);
  computeYaxes();
}

void GraphWindow::normalize() {
  float xMin = m_xMin;
  float xMax = m_xMax;
  float yMin = m_yMin;
  float yMax = m_yMax;
  m_xMin = (xMin+xMax)/2 - 5.3f;
  m_xMax = (xMin+xMax)/2 + 5.3f;
  m_xGridUnit = computeGridUnit(Axis::X);
  m_yAuto = false;
  m_yMin = (yMin+yMax)/2 - 3.1f;
  m_yMax = (yMin+yMax)/2 + 3.1f;
  m_yGridUnit = computeGridUnit(Axis::Y);
}

void GraphWindow::setDefault() {
  m_xMin = -10.0f;
  m_xMax = 10.0f;
  m_xGridUnit = computeGridUnit(Axis::X);
  setYAuto(true);
}

bool GraphWindow::panToMakePointVisible(float x, float y, float xMargin, float yMargin) {
  bool windowMoved = false;
  float xRange = m_xMax - m_xMin;
  float yRange = m_yMax - m_yMin;
  if (x < m_xMin + xMargin) {
    m_xMin = x - xMargin;
    m_xMax = m_xMin + xRange;
    m_xGridUnit = computeGridUnit(Axis::X);
    computeYaxes();
    windowMoved = true;
  }
  if (x > m_xMax - xMargin) {
    m_xMax = x + xMargin;
    m_xMin = m_xMax - xRange;
    m_xGridUnit = computeGridUnit(Axis::X);
    computeYaxes();
    windowMoved = true;
  }
  if (y < m_yMin + yMargin) {
    m_yMin = y - yMargin;
    m_yMax = m_yMin + yRange;
    m_yGridUnit = computeGridUnit(Axis::Y);
    windowMoved = true;
  }
  if (y > m_yMax - yMargin) {
    m_yMax = y + yMargin;
    m_yMin = m_yMax - yRange;
    m_yGridUnit = computeGridUnit(Axis::Y);
    windowMoved = true;
  }
  return windowMoved;
}

}
