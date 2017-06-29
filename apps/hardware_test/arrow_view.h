#ifndef HARDWARE_TEST_ARROW_VIEW_H
#define HARDWARE_TEST_ARROW_VIEW_H

#include <escher.h>

namespace HardwareTest {

class ArrowView : public View {
public:
  ArrowView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setDirection(bool up);
  void setColor(KDColor color);
private:
  constexpr static KDCoordinate k_arrowHeight = 10;
  constexpr static KDCoordinate k_arrowWidth = 9;
  constexpr static KDCoordinate k_arrowThickness = 3;
  bool m_directionIsUp;
  KDColor m_color;
};

}

#endif

