#ifndef HARDWARE_TEST_APP_H
#define HARDWARE_TEST_APP_H

#include <escher/bank_view_controller.h>

#include "../shared/shared_app.h"
#include "battery_test_controller.h"
#include "colors_lcd_test_controller.h"
#include "dead_pixels_test_controller.h"
#include "keyboard_test_controller.h"
#include "lcd_data_test_controller.h"
#include "lcd_timing_test_controller.h"
#include "led_test_controller.h"
#include "serial_number_controller.h"
#include "vblank_test_controller.h"

namespace HardwareTest {

class App : public Escher::App {
 public:
  class Snapshot : public Shared::SharedApp::Snapshot {
   public:
    App* unpack(Escher::Container* container) override;
    const Descriptor* descriptor() const override;
  };

 private:
  class WizardViewController : public Escher::BankViewController {
   public:
    WizardViewController(Escher::Responder* parentResponder);
    int numberOfChildren() override;
    Escher::ViewController* childAtIndex(int i) override;
    bool handleEvent(Ion::Events::Event event) override;

   private:
    BatteryTestController m_batteryTestController;
    LCDTimingTestController m_lcdTimingTestController;
    ColorsLCDTestController m_colorsLCDTestController;
    DeadPixelsTestController m_deadPixelsTestController;
    KeyboardTestController m_keyboardController;
    LCDDataTestController m_lcdDataTestController;
    LEDTestController m_ledTestController;
    SerialNumberController m_serialNumberController;
    VBlankTestController m_vBlankTestController;
  };

  App(Snapshot* snapshot);
  WizardViewController m_wizardViewController;
};

}  // namespace HardwareTest

#endif
