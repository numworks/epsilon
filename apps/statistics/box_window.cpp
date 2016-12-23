#include "box_window.h"

namespace Statistics {

BoxWindow::BoxWindow(Data * data) :
  m_data(data)
{
}

float BoxWindow::xMin() {
  return m_data->minValue();
}

float BoxWindow::xMax() {
  return m_data->maxValue();
}

float BoxWindow::yMin() {
  return 0.0f;
}

float BoxWindow::yMax() {
  return 1.0f;
}

float BoxWindow::xGridUnit() {
  return computeGridUnit(Axis::X, m_data->minValue(), m_data->maxValue());
}

}
