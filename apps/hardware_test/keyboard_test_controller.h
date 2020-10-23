#ifndef HARDWARE_TEST_KEYBOARD_CONTROLLER_H
#define HARDWARE_TEST_KEYBOARD_CONTROLLER_H

#include "keyboard_view.h"

namespace HardwareTest {

class KeyboardTestController : public Escher::ViewController {
public:
  KeyboardTestController(Escher::Responder * parentResponder);
  Escher::View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  KeyboardView m_keyboardView;
};

}

#endif

