#include "../shared/usb/calculator.h"
#include "display.h"
#include <ion.h>

void ion_main(int argc, char * argv[]) {
  Flasher::Display::init();
  while (true) {
    Ion::USB::enable();
    while (!Ion::USB::isEnumerated()) {
    }
    Ion::Device::USB::Calculator::PollAndReset(false);
  }
}
