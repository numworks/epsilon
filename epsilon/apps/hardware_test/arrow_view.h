#ifndef HARDWARE_TEST_ARROW_VIEW_H
#define HARDWARE_TEST_ARROW_VIEW_H

#include <escher/arbitrary_shaped_view.h>
#include <ion.h>
#include <omg/directions.h>

namespace HardwareTest {

class ArrowView : public Escher::ArbitraryShapedView {
 public:
  ArrowView();
  void drawRect(KDContext* ctx, KDRect rect) const override;

 private:
  constexpr static KDCoordinate k_arrowHeight = 10;
  constexpr static KDCoordinate k_arrowWidth = 9;
  constexpr static KDCoordinate k_arrowThickness = 3;
  OMG::VerticalDirection m_direction;
  KDColor m_color;
};

}  // namespace HardwareTest

#endif
