#include "box_range.h"
#include <apps/shared/range_1D.h>

namespace Statistics {

BoxRange::BoxRange(Store * store) :
  m_store(store)
{
}

float BoxRange::computeMinMax(bool isMax) const {
  float min = m_store->minValueForAllSeries(false, Shared::DoublePairStore::DefaultValidSeries);
  float max = m_store->maxValueForAllSeries(false, Shared::DoublePairStore::DefaultValidSeries);
  max = Shared::Range1D::checkedMax(max, nullptr, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat);
  min = Shared::Range1D::checkedMin(min, &max, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat);
  float margin = k_displayRightMarginRatio * (max - min);
  return isMax ? max + margin : min - margin;
}

}
