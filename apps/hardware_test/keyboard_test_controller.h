#ifndef HARDWARE_TEST_KEYBOARD_CONTROLLER_H
#define HARDWARE_TEST_KEYBOARD_CONTROLLER_H

#include <escher.h>
#include "keyboard_view.h"

namespace HardwareTest {

class KeyboardTestController : public ViewController {
public:
  KeyboardTestController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  KeyboardView m_keyboardView;
};

}

#endif

