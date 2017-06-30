#ifndef HARDWARE_TEST_APP_H
#define HARDWARE_TEST_APP_H

#include <escher.h>
#include "usb_test_controller.h"
#include "keyboard_test_controller.h"

class AppsContainer;

namespace HardwareTest {

class App : public ::App {
public:
  class Snapshot : public ::App::Snapshot {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };
  ViewController * USBController();
  int numberOfTimers() override;
  Timer * timerAtIndex(int i) override;
  bool processEvent(Ion::Events::Event e) override;
private:
  App(Container * container, Snapshot * snapshot);
  KeyboardTestController m_keyboardController;
  USBTestController m_USBTestController;
};

}

#endif

