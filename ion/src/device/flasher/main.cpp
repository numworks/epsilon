#include "../shared/usb/calculator.h"
#include <ion.h>

void ion_main(int argc, const char * const argv[]) {
  // Initialize the backlight
  Ion::Backlight::init();
  // Initialize Flasher display
  Ion::Device::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColorYellow);
  while (true) {
    Ion::USB::enable();
    while (!Ion::USB::isEnumerated()) {
    }
    Ion::USB::DFU(false);
  }
}
