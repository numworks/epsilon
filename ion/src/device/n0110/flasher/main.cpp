#include <boot/main.h>
#include "../shared/usb/calculator.h"
#include <ion.h>
#include <drivers/display.h>

void ion_main() {
  // Initialize Flasher display
  Ion::Device::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColorYellow);
  waitForInstruction();
}

void waitForInstruction() {
  while (true) {
    Ion::USB::enable();
    while (!Ion::USB::isEnumerated()) {
    }
    Ion::USB::DFU();
  }
}
