#ifndef HARDWARE_TEST_KEYBOARD_CONTROLLER_H
#define HARDWARE_TEST_KEYBOARD_CONTROLLER_H

#include <escher/view_controller.h>
#include "keyboard_view.h"

namespace HardwareTest {

class KeyboardTestController : public Escher::ViewController {
public:
  using Escher::ViewController::ViewController;
  Escher::View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  KeyboardView m_keyboardView;
};

}

#endif

