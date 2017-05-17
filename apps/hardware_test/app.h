#ifndef HARDWARE_TEST_APP_H
#define HARDWARE_TEST_APP_H

#include <escher.h>
#include "keyboard_controller.h"

class AppsContainer;

namespace HardwareTest {

class App : public ::App {
public:
  class Snapshot : public ::App::Snapshot {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };
private:
  App(Container * container, Snapshot * snapshot);
  KeyboardController m_keyboardController;
};

}

#endif

