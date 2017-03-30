#include "keyboard_controller.h"
extern "C" {
#include <assert.h>
}

namespace HardwareTest {

KeyboardController::KeyboardController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(KeyboardView())
{
}

View * KeyboardController::view() {
  return &m_view;
}

bool KeyboardController::handleEvent(Ion::Events::Event event) {
  if (event != Ion::Events::Event::PlainKey(m_view.testedKey()) && event != Ion::Events::Event::ShiftKey(m_view.testedKey()) && event != Ion::Events::Event::AlphaKey(m_view.testedKey()) && event != Ion::Events::Event::ShiftAlphaKey(m_view.testedKey())) {
    m_view.setDefectiveKey(m_view.testedKey());
  }
  m_view.setNextKey();
  return true;
}

void KeyboardController::viewWillAppear() {
  m_view.resetTestedKey();
}

}

