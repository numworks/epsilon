#pragma once

#include <escher/view_controller.h>

#include "keyboard_view.h"

namespace HardwareTest {

class KeyboardTestController : public Escher::ViewController {
 public:
  using Escher::ViewController::ViewController;
  Escher::View* view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  KeyboardTestController(Responder* parentResponder, bool testAllKeys)
      : Escher::ViewController(parentResponder), m_keyboardView(testAllKeys) {}

 private:
  KeyboardView m_keyboardView;
};

}  // namespace HardwareTest
