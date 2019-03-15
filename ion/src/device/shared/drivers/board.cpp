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
  CM4.CPACR()->setAccess(10, CM4::CPACR::Access::Full);
  CM4.CPACR()->setAccess(11, CM4::CPACR::Access::Full);
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

}
}
}
