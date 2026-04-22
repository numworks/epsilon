#pragma once

#include <escher/solid_color_view.h>
#include <escher/theme/components.h>
#include <escher/view_controller.h>

namespace HardwareTest {

class DeadPixelsTestController : public Escher::ViewController {
 public:
  DeadPixelsTestController(Escher::Responder* parentResponder)
      : Escher::ViewController(parentResponder),
        m_colorIndex(0),
        m_view(Escher::ThemeComponent::BasicBlack) {}
  Escher::View* view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;

 private:
  constexpr static int k_numberOfAdditionalColors = 4;
  // ThemeComponent::BasicBlack is the first color, set in the constructor
  constexpr static Escher::ThemeComponent k_colors[k_numberOfAdditionalColors] =
      {Escher::ThemeComponent::BasicRed, Escher::ThemeComponent::BasicBlue,
       Escher::ThemeComponent::BasicGreen, Escher::ThemeComponent::BasicWhite};
  int m_colorIndex;
  Escher::SolidColorView m_view;
};

}  // namespace HardwareTest
