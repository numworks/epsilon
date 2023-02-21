#ifndef STATISTICS_BOX_RANGE_H
#define STATISTICS_BOX_RANGE_H

#include <apps/shared/curve_view_range.h>

#include "../store.h"

namespace Statistics {

class BoxRange : public Shared::CurveViewRange {
 public:
  BoxRange(Store* store);
  float xMin() const override { return computeMinMax(false); }
  float xMax() const override { return computeMinMax(true); }
  float yMin() const override { return -1.0 - k_displayBottomMarginRatio; }
  float yMax() const override { return k_displayTopMarginRatio; }

 private:
  constexpr static float k_displayTopMarginRatio = 0.1f;
  constexpr static float k_displayRightMarginRatio = 0.2f;
  constexpr static float k_displayBottomMarginRatio = 0.1f;
  constexpr static float k_displayLeftMarginRatio = 0.2f;
  float computeMinMax(bool isMax) const;
  Store* m_store;
};

}  // namespace Statistics

#endif
