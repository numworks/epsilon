#pragma once

#include <escher/solid_color_view.h>
#include <escher/view_controller.h>
#include <kandinsky/basic_color.h>

namespace HardwareTest {

class DeadPixelsTestController : public Escher::ViewController {
 public:
  DeadPixelsTestController(Escher::Responder* parentResponder)
      : Escher::ViewController(parentResponder),
        m_colorIndex(0),
        m_view(BasicColor::Black) {}
  Escher::View* view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;

 private:
  constexpr static int k_numberOfAdditionalColors = 4;
  // BasicColor::Black is the first color, set in the constructor
  constexpr static KDColor k_colors[k_numberOfAdditionalColors] = {
      BasicColor::Red, BasicColor::Blue, BasicColor::Green, BasicColor::White};
  int m_colorIndex;
  Escher::RawColorSolidColorView m_view;
};

}  // namespace HardwareTest
