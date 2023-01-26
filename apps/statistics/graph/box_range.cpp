#include "box_range.h"

namespace Statistics {

BoxRange::BoxRange(Store * store) :
  m_store(store)
{}

float BoxRange::computeMinMax(bool isMax) const {
  Poincare::Range1D range1D;
  range1D.setMin(m_store->minValueForAllSeries(false, Shared::DoublePairStore::DefaultActiveSeriesTest));
  range1D.setMax(m_store->maxValueForAllSeries(false, Shared::DoublePairStore::DefaultActiveSeriesTest));
  float max = range1D.max();
  float min = range1D.min();
  float margin = k_displayRightMarginRatio * (max - min);
  return isMax ? max + margin : min - margin;
}

}
