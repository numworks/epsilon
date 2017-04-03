#ifndef HARDWARE_TEST_KEYBOARD_CONTROLLER_H
#define HARDWARE_TEST_KEYBOARD_CONTROLLER_H

#include <escher.h>
#include "keyboard_view.h"

namespace HardwareTest {

class KeyboardController : public ViewController {
public:
  KeyboardController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  static KDColor nextColor(KDColor color);
  KeyboardView m_view;
  KDColor m_color;
};

}

#endif

