#ifndef APPS_PROBABILITY_GUI_SPACER_VIEW_H
#define APPS_PROBABILITY_GUI_SPACER_VIEW_H

#include "solid_color_cell.h"

namespace Probability {

class SpacerView : public SolidColorCell {
public:
  SpacerView(KDColor color, KDCoordinate width, KDCoordinate height) :
      SolidColorCell(color), m_width(width), m_height(height) {}
  KDSize minimalSizeForOptimalDisplay() const override { return KDSize(m_width, m_height); }

private:
  KDCoordinate m_width, m_height;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_SPACER_VIEW_H */
