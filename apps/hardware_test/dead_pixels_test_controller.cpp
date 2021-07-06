#include "dead_pixels_test_controller.h"

namespace HardwareTest {

constexpr KDColor DeadPixelsTestController::k_colors[DeadPixelsTestController::k_numberOfAdditionalColors];

bool DeadPixelsTestController::handleEvent(Ion::Events::Event event) {
  // Do not handle OnOff event to let the apps container redraw the screen
  if (event == Ion::Events::OnOff) {
    return false;
  }
  if (event != Ion::Events::OK) {
    return true;
  }
  if (m_colorIndex == k_numberOfAdditionalColors) {
    // Go to the next step - this will be handled by the WizardViewController
    return false;
  } else {
    m_colorIndex++;
    m_view.setColor(k_colors[m_colorIndex - 1]);
  }
  return true;
}

}

