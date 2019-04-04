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

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void shutdown() {
  shutdownPeripherals();
  shutdownClocks();
}

void initFPU() {
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BABDBFBJ.html
  CORTEX.CPACR()->setAccess(10, CORTEX::CPACR::Access::Full);
  CORTEX.CPACR()->setAccess(11, CORTEX::CPACR::Access::Full);
  // FIXME: The pipeline should be flushed at this point
}

void initPeripherals() {
  Display::init();
  Backlight::init();
  Keyboard::init();
  LED::init();
  Battery::init();
  USB::init();
  Console::init();
  SWD::init();
  Timing::init();
  ExternalFlash::init();
}

void shutdownPeripherals(bool keepLEDAwake) {
  ExternalFlash::shutdown();
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
}

void setClockFrequency(Frequency f) {
  switch (f) {
    case Frequency::High:
      RCC.CFGR()->setHPRE(RCC::CFGR::AHBPrescaler::SysClk);
      return;
    default:
      assert(f == Frequency::Low);
      /* We could divide the system clock by 512. However, the HCLK clock
       * frequency must be >= 14.2MHz and <=216 MHz which forces the
       * AHBPrescaler to be below 192MHz/14.2MHz~13.5MHz. */
      RCC.CFGR()->setHPRE(RCC::CFGR::AHBPrescaler::SysClkDividedBy8);
      return;
  }
}

}
}
}
