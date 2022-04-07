#include <bootloader/boot.h>
#include <bootloader/slot.h>
#include <ion.h>
#include <ion/src/device/shared/drivers/reset.h>
#include <bootloader/interface.h>
#include <ion/src/device/n0110/drivers/power.h>
#include <bootloader/recovery.h>
#include <bootloader/usb_data.h>
#include <ion/src/device/shared/drivers/flash.h>

#include <assert.h>

namespace Bootloader {

BootMode Boot::mode() {
  return BootMode::SlotA;
}

void Boot::setMode(BootMode mode) {
  // We dont use the exam mode driver as storage for the boot mode because we need the 16k of storage x)
}

void Boot::bootSlot(Bootloader::Slot s) {
  if (!s.userlandHeader()->isOmega() && !s.userlandHeader()->isUpsilon()) {
    // We are trying to boot epsilon, so we check the version and show an advertisement if needed
    const char * version = s.userlandHeader()->version();
    const char * min = "18.2.4";
    int vsum = 0;
    for (int i = 0; i < strlen(version); i++) {
      vsum += version[i] * (100-i*15);
    }
    int minsum = 0;
    for (int i = 0; i < strlen(min); i++) {
      minsum += min[i] * (100-i*15);
    }
    if (vsum >= minsum) {
      Interface::drawEpsilonAdvertisement();
      uint64_t scan = 0;
      while (scan != Ion::Keyboard::State(Ion::Keyboard::Key::Back)) {
        scan = Ion::Keyboard::scan();
        if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::EXE) || scan == Ion::Keyboard::State(Ion::Keyboard::Key::OK)) {
          scan = Ion::Keyboard::State(Ion::Keyboard::Key::Back);
          s.boot();
        } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::OnOff)) {
          Ion::Power::standby(); // Force a core reset to exit
        }
      }
      Interface::drawMenu();
      return;
    }
  }
  s.boot();
  Interface::drawMenu();
}

__attribute__((noreturn)) void Boot::boot() {
  assert(mode() != BootMode::Unknown);

  bool isSlotA = Slot::A().kernelHeader()->isValid();
  bool isSlotB = Slot::B().kernelHeader()->isValid();
  bool isSlotKhi = Slot::Khi().kernelHeader()->isValid();

  Interface::drawMenu();

  while (true) {
    uint64_t scan = Ion::Keyboard::scan();
    if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::One) && isSlotA) {
      Boot::bootSlot(Slot::A());
    } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Two) && isSlotKhi) {
      Boot::bootSlot(Slot::Khi());
    } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Three) && isSlotB) {
      Boot::bootSlot(Slot::B());
    } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Four)) {
      installerMenu();
    } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Five)) {
      aboutMenu();
    }
    // else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Six)) {
    //   Ion::Device::Reset::core();
    // }
     else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::OnOff)) {
      Ion::Power::standby(); // Force a core reset to exit
    }
  }

  // Achievement unlocked: How Did We Get Here?
  bootloader();
}

void Boot::installerMenu() {
  Interface::drawInstallerSelection();
  uint64_t scan = 0;
  while (scan != Ion::Keyboard::State(Ion::Keyboard::Key::Back)) {
    scan = Ion::Keyboard::scan();
    if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::One)) {
      scan = Ion::Keyboard::State(Ion::Keyboard::Key::Back);
      bootloader();
      continue;
    } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Two)) {
      scan = Ion::Keyboard::State(Ion::Keyboard::Key::Back);
      bootloaderUpdate();
      continue;
    } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::OnOff)) {
      Ion::Power::standby(); // Force a core reset to exit
    }
  }

  Interface::drawMenu();
}

void Boot::aboutMenu() {
  // Draw the about menu
  Interface::drawAbout();
  // Wait for the user to press OK, EXE or Back button
  while (true) {
    uint64_t scan = Ion::Keyboard::scan();
    if ((scan == Ion::Keyboard::State(Ion::Keyboard::Key::OK)) ||
        (scan == Ion::Keyboard::State(Ion::Keyboard::Key::EXE)) ||
        (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Back))) {
      // Redraw the menu and return
      Interface::drawMenu();
      return;
    }  else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::OnOff)) {
      Ion::Power::standby(); // Force a core reset to exit
    }
  }

}

void Boot::bootloaderUpdate() {
  USBData data = USBData::BOOTLOADER_UPDATE();
  
  for (;;) {
    Bootloader::Interface::drawBootloaderUpdate();
    Ion::USB::enable();
    do {
      uint64_t scan = Ion::Keyboard::scan();
      if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Back)) {
        Ion::USB::disable();
        Interface::drawMenu();
        return;
      } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::OnOff)) {
        Ion::Power::standby();
      }
    } while (!Ion::USB::isEnumerated());

    Ion::USB::DFU(true, &data);
  }
}

void Boot::bootloader() {
  USBData data = USBData::DEFAULT();
  for(;;) {
    // Draw the interfaces and infos
    Bootloader::Interface::drawFlasher();

    // Enable USB
    Ion::USB::enable();

    // Wait for the device to be enumerated
    do {
      // If we pressed back while waiting, reset.
      uint64_t scan = Ion::Keyboard::scan();
      if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::Back)) {
        // Disable USB, redraw the menu and return
        Ion::USB::disable();
        Interface::drawMenu();
        return;
      } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::OnOff)) {
        Ion::Power::standby(); // Force a core reset to exit
      }
    } while (!Ion::USB::isEnumerated());

    // Launch the DFU stack, allowing to press Back to quit and reset
    Ion::USB::DFU(true, &data);
  }
}

void Boot::lockInternal() {
  Ion::Device::Flash::DisableInternalProtection();
  Ion::Device::Flash::SetInternalSectorProtection(0, true);
  Ion::Device::Flash::SetInternalSectorProtection(1, true);
  Ion::Device::Flash::SetInternalSectorProtection(2, true);
  Ion::Device::Flash::SetInternalSectorProtection(3, true);
  Ion::Device::Flash::EnableInternalProtection();
}

}
