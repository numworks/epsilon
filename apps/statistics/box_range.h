#ifndef STATISTICS_BOX_RANGE_H
#define STATISTICS_BOX_RANGE_H

#include "../shared/curve_view_range.h"
#include "store.h"

namespace Statistics {

class BoxRange : public Shared::CurveViewRange {
public:
  BoxRange(Store * store);
  float xMin() override;
  float xMax() override;
  float yMin() override;
  float yMax() override;
  float xGridUnit() override;
private:
  constexpr static float k_displayTopMarginRatio = 0.05f;
  constexpr static float k_displayRightMarginRatio = 0.2f;
  constexpr static float k_displayBottomMarginRatio = 0.2f;
  constexpr static float k_displayLeftMarginRatio = 0.2f;
  Store * m_store;
};

}

#endif