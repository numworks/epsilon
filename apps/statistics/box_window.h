#ifndef STATISTICS_BOX_WINDOW_H
#define STATISTICS_BOX_WINDOW_H

#include "../curve_view_window.h"
#include "store.h"

namespace Statistics {

class BoxWindow : public CurveViewWindow {
public:
  BoxWindow(Store * store);
  float xMin() override;
  // if the range of value is to wide compared to the bar width, value max is capped
  float xMax() override;
  float yMin() override;
  float yMax() override;
  float xGridUnit() override;
private:
  Store * m_store;

};

#endif

}
