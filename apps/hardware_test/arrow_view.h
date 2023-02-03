#ifndef HARDWARE_TEST_ARROW_VIEW_H
#define HARDWARE_TEST_ARROW_VIEW_H

#include <escher/transparent_view.h>
#include <ion.h>
#include <omg/directions.h>

namespace HardwareTest {

class ArrowView : public Escher::TransparentView {
public:
  ArrowView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setDirection(OMG::VerticalDirection direction);
  void setColor(KDColor color);
private:
  constexpr static KDCoordinate k_arrowHeight = 10;
  constexpr static KDCoordinate k_arrowWidth = 9;
  constexpr static KDCoordinate k_arrowThickness = 3;
  OMG::VerticalDirection m_direction;
  KDColor m_color;
};

}

#endif

