#pragma once

#include <escher/app.h>
#include <escher/bank_view_controller.h>

#include "battery_test_controller.h"
#include "colors_lcd_test_controller.h"
#include "dead_pixels_test_controller.h"
#include "group_test_controller.h"
#include "keyboard_test_controller.h"
#include "lcd_data_test_controller.h"
#include "lcd_timing_test_controller.h"
#include "led_test_controller.h"
#include "serial_number_controller.h"
#include "vblank_test_controller.h"

namespace HardwareTest {

enum Variant { Default = 0, TestAllKeys = 1 };

class App : public Escher::App {
 public:
  constexpr static size_t k_numberOfTests = 9;

  // Tests that run in the GroupTestController batch
  constexpr static size_t k_numberOfGroupedTests = 4;
  constexpr static size_t k_numberOfTestRuns =
      k_numberOfTests - k_numberOfGroupedTests + 1;

  class Snapshot : public Escher::App::Snapshot {
   public:
    App* unpack(Escher::Container* container) override;
    const Descriptor* descriptor() const override;
    Variant variant() const { return m_variant; }
    void setVariant(Variant variant) { m_variant = variant; }

   private:
    Variant m_variant = Variant::Default;
  };

 private:
  class WizardViewController : public Escher::BankViewController {
   public:
    WizardViewController(Escher::Responder* parentResponder, Variant variant);
    int numberOfChildren() override;
    Escher::ViewController* childAtIndex(int i) override;
    bool handleEvent(Ion::Events::Event event) override;

   private:
    BatteryTestController m_batteryTestController;
    LCDTimingTestController m_lcdTimingTestController;
    ColorsLCDTestController m_colorsLCDTestController;
    DeadPixelsTestController m_deadPixelsTestController;
    KeyboardTestController m_keyboardTestController;
    LCDDataTestController m_lcdDataTestController;
    LEDTestController m_ledTestController;
    SerialNumberController m_serialNumberController;
    VBlankTestController m_vBlankTestController;

    GroupTestController<k_numberOfGroupedTests> m_groupTestController;
  };

  App(Snapshot* snapshot);
  WizardViewController m_wizardViewController;
};

}  // namespace HardwareTest
