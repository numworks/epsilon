#include "../shared/usb/calculator.h"
#include "display.h"
#include <ion.h>

void ion_main(int argc, const char * const argv[]) {
  // Initialize the backlight
  Ion::Backlight::init();
  // Initialize Flasher display
  Flasher::Display::init();
  while (true) {
    Ion::USB::enable();
    while (!Ion::USB::isEnumerated()) {
    }
    Ion::USB::DFU(false);
  }
}
