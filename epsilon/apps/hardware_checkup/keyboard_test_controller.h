#pragma once

#include <escher/view_controller.h>

#include "keyboard_view.h"

namespace HardwareCheckup {

class KeyboardTestController : public Escher::ViewController {
 public:
  using Escher::ViewController::ViewController;
  Escher::View* view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;

 private:
  KeyboardView m_keyboardView;
};

}  // namespace HardwareCheckup
