#include "box_window.h"

namespace Statistics {

BoxWindow::BoxWindow(Store * store) :
  m_store(store)
{
}

float BoxWindow::xMin() {
  return m_store->minValue();
}

float BoxWindow::xMax() {
  if (m_store->minValue() >= m_store->maxValue()) {
    return m_store->minValue() + 1.0f;
  }
  return m_store->maxValue();
}

float BoxWindow::yMin() {
  return 0.0f;
}

float BoxWindow::yMax() {
  return 1.0f;
}

float BoxWindow::xGridUnit() {
  return computeGridUnit(Axis::X, xMin(), xMax());
}

}
