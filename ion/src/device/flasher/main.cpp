#include "../shared/usb/calculator.h"
#include <ion.h>
#include "drivers/backlight.h"
#include <drivers/display.h>

void flasher_main(int argc, const char * const argv[]) {
  // Initialize the backlight
  Ion::Device::Backlight::init();
  // Initialize Flasher display
  Ion::Device::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColorYellow);
  while (true) {
    Ion::USB::enable();
    while (!Ion::USB::isEnumerated()) {
    }
    Ion::USB::DFU(false);
  }
}
