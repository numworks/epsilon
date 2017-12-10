#include "box_range.h"

namespace Statistics {

float BoxRange::xMin() {
  float min = m_store->minValueOfColumn(0);
  float max = m_store->maxValueOfColumn(0);
  max = min >= max ? min + 1 : max;
  return min - k_displayLeftMarginRatio*(max-min);
}

float BoxRange::xMax() {
  float min = m_store->minValueOfColumn(0);
  float max = m_store->maxValueOfColumn(0);
  max = min >= max ? min + 1 : max;
  return max + k_displayRightMarginRatio*(max - min);
}

float BoxRange::yMin() {
  return -k_displayBottomMarginRatio;
}

float BoxRange::yMax() {
  return 1.0f+k_displayTopMarginRatio;
}

float BoxRange::xGridUnit() {
  return computeGridUnit(Axis::X, xMin(), xMax());
}

}
