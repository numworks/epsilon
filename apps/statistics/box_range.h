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
  Store * m_store;
};

}

#endif