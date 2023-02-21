#include "keyboard_test_controller.h"
extern "C" {
#include <assert.h>
}

using namespace Escher;

namespace HardwareTest {

View* KeyboardTestController::view() { return &m_keyboardView; }

bool KeyboardTestController::handleEvent(Ion::Events::Event event) {
  // Do not handle OnOff event to let the apps container redraw the screen
  if (event == Ion::Events::OnOff) {
    return false;
  }
  Ion::Keyboard::State state = Ion::Keyboard::scan();
  Ion::Keyboard::State onlyKeyDown = Ion::Keyboard::State(
      KeyboardModel::TestedKeys[m_keyboardView.testedKeyIndex()]);
  if (state == onlyKeyDown) {
    m_keyboardView.setTestedKeyIndex(m_keyboardView.testedKeyIndex() + 1);
    if (m_keyboardView.testedKeyIndex() == KeyboardModel::NumberOfTestedKeys) {
      // Returning false will go to the next step in the WizardViewController
      return false;
    }
  }
  return true;
}

void KeyboardTestController::viewWillAppear() {
  m_keyboardView.setTestedKeyIndex(0);
}

}  // namespace HardwareTest
