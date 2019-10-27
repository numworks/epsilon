#include "box_range.h"

namespace Statistics {

BoxRange::BoxRange(Store * store) :
  m_store(store)
{
}

float BoxRange::xMin() const {
  float min = m_store->minValueForAllSeries();
  float max = m_store->maxValueForAllSeries();
  max = min >= max ? min + 1 : max;
  return min - k_displayLeftMarginRatio*(max-min);
}

float BoxRange::xMax() const {
  float min = m_store->minValueForAllSeries();
  float max = m_store->maxValueForAllSeries();
  max = min >= max ? min + 1 : max;
  return max + k_displayRightMarginRatio*(max - min);
}

}
