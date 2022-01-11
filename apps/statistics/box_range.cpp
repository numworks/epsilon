#include "box_range.h"

namespace Statistics {

BoxRange::BoxRange(Store * store) :
  m_store(store)
{
}

float BoxRange::computeMinMax(bool isMax) const {
  float min = m_store->minValueForAllSeries();
  float max = m_store->maxValueForAllSeries();
  max = Shared::Range1D::checkedMax(max, nullptr, Store::k_lowerMaxFloat, Store::k_upperMaxFloat);
  min = Shared::Range1D::checkedMin(min, &max, Store::k_lowerMaxFloat, Store::k_upperMaxFloat);
  float margin = k_displayRightMarginRatio * (max - min);
  return isMax ? max + margin : min - margin;
}

}
