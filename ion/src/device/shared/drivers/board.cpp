#include "board.h"
#include <drivers/board.h>
#include <drivers/backlight.h>
#include <drivers/battery.h>
#include <drivers/console.h>
#include <drivers/display.h>
#include <drivers/external_flash.h>
#include <drivers/keyboard.h>
#include <drivers/led.h>
#include <drivers/swd.h>
#include <drivers/timing.h>
#include <drivers/usb.h>
#include <drivers/config/clocks.h>

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void initFPU() {
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BABDBFBJ.html
  CORTEX.CPACR()->setAccess(10, CORTEX::CPACR::Access::Full);
  CORTEX.CPACR()->setAccess(11, CORTEX::CPACR::Access::Full);
  // FIXME: The pipeline should be flushed at this point
}

void initCompensationCell() {
  /* The output speed of some GPIO pins is set to high, in which case,
   * the compensation cell should be enabled. */
  SYSCFG.CMPCR()->setCMP_PD(true);
  while (!SYSCFG.CMPCR()->getREADY()) {
  }
}

void shutdownCompensationCell() {
  SYSCFG.CMPCR()->setCMP_PD(false);
}

void initPeripherals(bool initBacklight) {
  initCompensationCell();
  Display::init();
  if (initBacklight) {
    Backlight::init();
  }
  Keyboard::init();
  LED::init();
  Battery::init();
  USB::init();
  Console::init();
  SWD::init();
  Timing::init();
}

void shutdownPeripherals(bool keepLEDAwake) {
  Timing::shutdown();
  SWD::shutdown();
  Console::shutdown();
  USB::shutdown();
  Battery::shutdown();
  if (!keepLEDAwake) {
    LED::shutdown();
  }
  Keyboard::shutdown();
  Backlight::shutdown();
  Display::shutdown();
  shutdownCompensationCell();
}

static Frequency sStandardFrequency = Frequency::High;

Frequency standardFrequency() {
  return sStandardFrequency;
}

void setStandardFrequency(Frequency f) {
  sStandardFrequency = f;
}

void setClockFrequency(Frequency f) {
  // TODO: Update TIM3 prescaler or ARR to avoid irregular LED blinking
  if (f == Frequency::High) {
    RCC.CFGR()->setHPRE(RCC::CFGR::AHBPrescaler::SysClk);
    Device::Timing::setSysTickFrequency(Ion::Device::Clocks::Config::HCLKFrequency);
  } else {
    assert(f == Frequency::Low);
    // Change the systick frequency to compensate the KCLK frequency change
    Device::Timing::setSysTickFrequency(Ion::Device::Clocks::Config::HCLKLowFrequency);
    RCC.CFGR()->setHPRE(Clocks::Config::AHBLowFrequencyPrescalerReg);
  }
}

}
}
}
