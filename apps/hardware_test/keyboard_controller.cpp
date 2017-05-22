#include "keyboard_controller.h"
#include "../apps_container.h"
extern "C" {
#include <assert.h>
}

namespace HardwareTest {

KeyboardController::KeyboardController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(),
  m_color(KDColorBlack),
  m_colorController(nullptr)
{
}

View * KeyboardController::view() {
  return &m_view;
}

bool KeyboardController::handleEvent(Ion::Events::Event event) {
  Ion::Keyboard::State state = Ion::Keyboard::scan();
  m_view.updateLEDState(m_color);
  m_color = nextColor(m_color);
  m_view.updateBatteryState(Ion::Battery::voltage(), Ion::Battery::isCharging());
  if (!Ion::Keyboard::keyDown(Ion::Keyboard::ValidKeys[m_view.testedKey()], state)) {
    m_view.setDefectiveKey(m_view.testedKey());
  }
  if (!m_view.setNextKey()) {
    m_view.updateLEDState(KDColorBlack);
    ModalViewController * modal = (ModalViewController *)parentResponder();
    modal->displayModalViewController(&m_colorController, 0.0f, 0.0f);
  }
  return true;
}

void KeyboardController::viewWillAppear() {
  m_view.resetTestedKey();
  m_color = KDColorBlack;
  m_view.updateLEDState(m_color);
  m_view.updateBatteryState(Ion::Battery::voltage(), Ion::Battery::isCharging());
}

KDColor KeyboardController::nextColor(KDColor color) {
  if (color == KDColorBlack) {
    return KDColorWhite;
  }
  if (color == KDColorWhite) {
    return KDColorRed;
  }
  if (color == KDColorRed) {
    return KDColorBlue;
  }
  if (color == KDColorBlue) {
    return KDColorGreen;
  }
  if (color == KDColorGreen) {
    return KDColorBlack;
  }
  return KDColorBlack;
}

}

