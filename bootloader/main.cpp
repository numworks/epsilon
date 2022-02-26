
#include <ion.h>
#include <bootloader/kernel_header.h>
#include <assert.h>
#include <ion/src/device/shared/drivers/board.h>

#include "interface.h"

void ion_main(int argc, const char * const argv[]) {
  // Clear the screen
  Ion::Display::pushRectUniform(KDRect(0,0,320,240), KDColorBlack);
  // Initialize the backlight
  Ion::Backlight::init();

  uint64_t scan = Ion::Keyboard::scan();

  if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Four)) {
    Bootloader::Interface::draw();
    while (true) {
      Ion::USB::enable();
      while (!Ion::USB::isEnumerated()) {
      }
      Ion::USB::DFU(false);
    }
  }

  Ion::Device::Board::bootloaderMPU();
  Bootloader::s_kernelHeaderA->boot();
}
