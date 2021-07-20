#include <drivers/backlight.h>
#include <drivers/battery.h>
#include <drivers/board_unprotected.h>
#include <drivers/cache.h>
#include <drivers/console.h>
#include <drivers/display.h>
#include <drivers/external_flash.h>
#include <drivers/keyboard_init.h>
#include <drivers/led.h>
#include <drivers/usb_privileged.h>

namespace Ion {
namespace Device {
namespace Board {

void initPeripherals() {
  initCompensationCell();
  LED::init();
  USB::init();
  Battery::init();
  Display::init();
  Keyboard::init();
  ExternalFlash::init();
  Backlight::init();
  Console::init();
}

void shutdownPeripherals() {
  Console::shutdown();
  Backlight::shutdown();
  ExternalFlash::shutdown();
  Keyboard::shutdown();
  Display::shutdown();
  Battery::shutdown();
  USB::shutdown();
  LED::shutdown();
  shutdownCompensationCell();
}

}
}
}
