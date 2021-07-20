#include <drivers/backlight.h>
#include <drivers/board_unprotected.h>
#include <drivers/display.h>
#include <drivers/external_flash.h>
#include <drivers/usb_privileged.h>

typedef void(*ISR)(void);
extern ISR InitialisationVector[];

namespace Ion {
namespace Device {
namespace Board {

void initPeripherals() {
  initCompensationCell();
  Backlight::init();
  USB::init();
  Display::init();
  ExternalFlash::init();
}

void shutdownPeripherals() {
  ExternalFlash::shutdown();
  Display::shutdown();
  USB::shutdown();
  Backlight::shutdown();
  shutdownCompensationCell();
}

}
}
}
