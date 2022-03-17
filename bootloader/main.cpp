
#include <ion.h>
#include <assert.h>

#include <bootloader/boot.h>

__attribute__ ((noreturn)) void ion_main(int argc, const char * const argv[]) {
  // Clear the screen
  Ion::Display::pushRectUniform(KDRect(0,0,320,240), KDColorBlack);
  // Initialize the backlight
  Ion::Backlight::init();

  // Set the mode to slot A if undefined
  if (Bootloader::Boot::mode() == Bootloader::BootMode::Unknown) {
    Bootloader::Boot::setMode(Bootloader::BootMode::SlotA);
  }

  // Handle rebooting to bootloader
  if (Bootloader::Boot::mode() == Bootloader::BootMode::SlotABootloader) {
    Bootloader::Boot::setMode(Bootloader::BootMode::SlotA);
    Bootloader::Boot::bootloader();
  } else if (Bootloader::Boot::mode() == Bootloader::BootMode::SlotBBootloader) {
    Bootloader::Boot::setMode(Bootloader::BootMode::SlotB);
    Bootloader::Boot::bootloader();
  }

  uint64_t scan = Ion::Keyboard::scan();

  // Reset+4 => Launch bootloader
  if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Four)) {
    Bootloader::Boot::bootloader();
  // Reset+1 => Launch slot A
  } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::One)) {
    Bootloader::Boot::setMode(Bootloader::BootMode::SlotA);
  // Reset+2 => Launch slot B
  } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Two)) {
    Bootloader::Boot::setMode(Bootloader::BootMode::SlotB);
  }

  // Boot the firmware
  Bootloader::Boot::boot();
}
