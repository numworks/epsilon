#include "keyboard_controller.h"
extern "C" {
#include <assert.h>
}

namespace HardwareTest {

KeyboardController::KeyboardController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(KeyboardView()),
  m_color(KDColorBlack)
{
}

View * KeyboardController::view() {
  return &m_view;
}

bool KeyboardController::handleEvent(Ion::Events::Event event) {
  Ion::LED::setColor(m_color);
  m_color = nextColor(m_color);
  m_view.updateBatteryState(Ion::Battery::voltage(), Ion::Battery::isCharging());
  if (event != Ion::Events::Event::PlainKey(m_view.testedKey()) && event != Ion::Events::Event::ShiftKey(m_view.testedKey()) && event != Ion::Events::Event::AlphaKey(m_view.testedKey()) && event != Ion::Events::Event::ShiftAlphaKey(m_view.testedKey())) {
    m_view.setDefectiveKey(m_view.testedKey());
  }
  m_view.setNextKey();
  return true;
}

void KeyboardController::viewWillAppear() {
  m_color = KDColorBlack;
  m_view.resetTestedKey();
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

