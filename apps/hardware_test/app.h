#ifndef HARDWARE_TEST_APP_H
#define HARDWARE_TEST_APP_H

#include <escher.h>
#include "keyboard_controller.h"

class AppsContainer;

namespace HardwareTest {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    App * build(Container * container) override;
  };
  static Descriptor * buildDescriptor();
private:
  App(Container * container, Descriptor * descriptor);
  KeyboardController m_keyboardController;
};

}

#endif

