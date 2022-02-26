
#include <ion.h>
#include <assert.h>
#include <ion/src/device/shared/drivers/board.h>

#include <bootloader/boot.h>

void ion_main(int argc, const char * const argv[]) {
  // Clear the screen
  Ion::Display::pushRectUniform(KDRect(0,0,320,240), KDColorBlack);
  // Initialize the backlight
  Ion::Backlight::init();

  if (Bootloader::Boot::mode() == Bootloader::BootMode::Unknown)
    Bootloader::Boot::setMode(Bootloader::BootMode::SlotA);

  if (Bootloader::Boot::mode() == Bootloader::BootMode::SlotABootloader) {
    Bootloader::Boot::setMode(Bootloader::BootMode::SlotA);
    Bootloader::Boot::bootloader();
  } else if (Bootloader::Boot::mode() == Bootloader::BootMode::SlotBBootloader) {
    Bootloader::Boot::setMode(Bootloader::BootMode::SlotB);
    Bootloader::Boot::bootloader();
  }

  uint64_t scan = Ion::Keyboard::scan();

  if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Four)) {
    Bootloader::Boot::bootloader();
  } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::One)) {
    Bootloader::Boot::setMode(Bootloader::BootMode::SlotA);
  } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Two)) {
    Bootloader::Boot::setMode(Bootloader::BootMode::SlotB);
  }

  Ion::Device::Board::bootloaderMPU();
  Bootloader::Boot::boot();
}
