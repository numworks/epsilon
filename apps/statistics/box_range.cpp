#include "box_range.h"

namespace Statistics {

BoxRange::BoxRange(Store * store) :
  m_store(store)
{
}

float BoxRange::xMin() {
  return m_store->minValue();
}

float BoxRange::xMax() {
  if (m_store->minValue() >= m_store->maxValue()) {
    return m_store->minValue() + 1.0f;
  }
  return m_store->maxValue();
}

float BoxRange::yMin() {
  return 0.0f;
}

float BoxRange::yMax() {
  return 1.0f;
}

float BoxRange::xGridUnit() {
  return computeGridUnit(Axis::X, xMin(), xMax());
}

}
