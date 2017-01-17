#ifndef STATISTICS_BOX_RANGE_H
#define STATISTICS_BOX_RANGE_H

#include "../curve_view_range.h"
#include "store.h"

namespace Statistics {

class BoxRange : public CurveViewRange {
public:
  BoxRange(Store * store);
  float xMin() override;
  float xMax() override;
  float yMin() override;
  float yMax() override;
  float xGridUnit() override;
private:
  constexpr static float k_displayTopMarginRatio = 0.0f;
  constexpr static float k_displayRightMarginRatio = 0.2f;
  constexpr static float k_displayBottomMarginRatio = 0.4f;
  constexpr static float k_displayLeftMarginRatio = 0.2f;
  Store * m_store;
};

}

#endif