#ifndef HARDWARE_TEST_APP_H
#define HARDWARE_TEST_APP_H

#include <escher.h>
#include "keyboard_test_controller.h"
#include "screen_test_controller.h"
#include "led_test_controller.h"
#include "battery_test_controller.h"
#include "serial_number_controller.h"

class AppsContainer;

namespace HardwareTest {

class App final : public ::App {
public:
  class Snapshot final : public ::App::Snapshot {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override {
      return &s_descriptor;
    }
  private:
    static App::Descriptor s_descriptor;
  };
private:
  class WizardViewController final : public BankViewController {
  public:
    WizardViewController(Responder * parentResponder) :
      BankViewController(parentResponder),
      m_keyboardController(this),
      m_screenTestController(this),
      m_ledTestController(this),
      m_batteryTestController(this),
      m_serialNumberController(this) {}
    int numberOfChildren() override { return 5; }
    ViewController * childAtIndex(int i) override;
    bool handleEvent(Ion::Events::Event event) override;
  private:
    KeyboardTestController m_keyboardController;
    ScreenTestController m_screenTestController;
    LEDTestController m_ledTestController;
    BatteryTestController m_batteryTestController;
    SerialNumberController m_serialNumberController;
  };

  App(Container * container, Snapshot * snapshot) :
    ::App(container, snapshot, &m_wizardViewController),
    m_wizardViewController(&m_modalViewController) {}
  WizardViewController m_wizardViewController;
};

}

#endif

