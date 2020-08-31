#include "../shared/usb/calculator.h"
#include "display.h"
#include <ion.h>

void ion_main() {
  // Initialize the backlight
  Ion::Backlight::init();
  // Initialize Flasher display
  Flasher::Display::init();
  while (true) {
    Ion::USB::enable();
    while (!Ion::USB::isEnumerated()) {
    }
    Ion::Device::USB::Calculator::PollAndReset(false);
  }
}
