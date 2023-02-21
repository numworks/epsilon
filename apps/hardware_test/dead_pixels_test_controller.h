#ifndef HARDWARE_TEST_DEAD_PIXELS_TEST_CONTROLLER_H
#define HARDWARE_TEST_DEAD_PIXELS_TEST_CONTROLLER_H

#include <escher/solid_color_view.h>
#include <escher/view_controller.h>

namespace HardwareTest {

class DeadPixelsTestController : public Escher::ViewController {
 public:
  DeadPixelsTestController(Escher::Responder* parentResponder)
      : Escher::ViewController(parentResponder),
        m_colorIndex(0),
        m_view(KDColorBlack) {}
  Escher::View* view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;

 private:
  constexpr static int k_numberOfAdditionalColors = 4;
  // KDColorBlack is the first color, set in the constructor
  constexpr static KDColor k_colors[k_numberOfAdditionalColors] = {
      KDColorRed, KDColorBlue, KDColorGreen, KDColorWhite};
  int m_colorIndex;
  Escher::SolidColorView m_view;
};

}  // namespace HardwareTest

#endif
