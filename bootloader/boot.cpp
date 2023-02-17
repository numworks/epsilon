#include <bootloader/boot.h>
#include <bootloader/slots/slot.h>
#include <bootloader/interface/static/interface.h>
#include <bootloader/recovery.h>
#include <bootloader/usb_data.h>
#include <bootloader/utility.h>
#include <bootloader/interface/menus/home.h>
#include <bootloader/interface/menus/warning.h>
#include <ion.h>
#include <ion/src/device/shared/drivers/flash.h>
#include <ion/src/device/shared/drivers/board.h>
#include <ion/src/device/shared/drivers/reset.h>
#include <ion/src/device/shared/drivers/external_flash.h>
#include <ion/src/device/n0110/drivers/power.h>

#include <assert.h>

using namespace Utility;

extern "C" {
  extern char _fake_isr_function_start;
}

namespace Bootloader {

BootConfig * Boot::config() {
  static BootConfig * bootcfg = new BootConfig();
  return bootcfg;
}

BootMode Boot::mode() {
  return BootMode::SlotA;
}

void Boot::setMode(BootMode mode) {
  // We dont use the exam mode driver as storage for the boot mode because we need the 16k of storage x)
}

void Boot::busError() {
  Ion::Device::Flash::ClearInternalFlashErrors();
  asm("mov r12, lr");
  if (config()->isBooting()) { // Bus error is normal if we are booting, it's triggered when we lock OPTCR
    asm("mov lr, r12");
    asm("bx lr");
  }
  Bootloader::Recovery::crash_handler("BusFault");
}

bool Boot::isKernelPatched(const Slot & s) {
  if (s.userlandHeader()->isOmega()) {
    // we don't need to patch the kernel
    return true;
  }

  // It's an epsilon kernel, so we need to check if it's patched

  uint32_t origin_isr = s.address() + sizeof(Bootloader::KernelHeader) - sizeof(uint32_t) * 3;

  if (*(uint32_t *)(origin_isr + sizeof(uint32_t) * 12) == (uint32_t)0x0) {
    // fake epsilon
    return true;
  }

  // return *(uint32_t *)(origin_isr + sizeof(uint32_t) * 4) == ((uint32_t)&_fake_isr_function_start) + 1 && *(uint32_t *)(origin_isr + sizeof(uint32_t) * 5) == ((uint32_t)&_fake_isr_function_start) + 1 && *(uint32_t *)(origin_isr + sizeof(uint32_t) * 6) == ((uint32_t)&_fake_isr_function_start) + 1 && *(uint32_t *)(origin_isr + sizeof(uint32_t) * 7) == ((uint32_t)&_fake_isr_function_start) + 1;*(uint32_t *)(origin_isr + sizeof(uint32_t) * 4) == ((uint32_t)&_fake_isr_function_start) + 1 && *(uint32_t *)(origin_isr + sizeof(uint32_t) * 5) == ((uint32_t)&_fake_isr_function_start) + 1 && *(uint32_t *)(origin_isr + sizeof(uint32_t) * 6) == ((uint32_t)&_fake_isr_function_start) + 1 && *(uint32_t *)(origin_isr + sizeof(uint32_t) * 7) == ((uint32_t)&_fake_isr_function_start) + 1;*(uint32_t *)(origin_isr + sizeof(uint32_t) * 4) == ((uint32_t)&_fake_isr_function_start) + 1 && *(uint32_t *)(origin_isr + sizeof(uint32_t) * 5) == ((uint32_t)&_fake_isr_function_start) + 1 && *(uint32_t *)(origin_isr + sizeof(uint32_t) * 6) == ((uint32_t)&_fake_isr_function_start) + 1 && *(uint32_t *)(origin_isr + sizeof(uint32_t) * 7) == ((uint32_t)&_fake_isr_function_start) + 1;
  return  *(uint32_t *)(origin_isr + sizeof(uint32_t) * 7) == ((uint32_t)&_fake_isr_function_start) + 1;
}

__attribute((section(".fake_isr_function"))) __attribute__((used)) void Boot::flash_interrupt() {
  // a simple function
  Ion::Device::Flash::ClearInternalFlashErrors();
  asm("bx lr");
}

void Boot::patchKernel(const Slot & s) {
  uint32_t origin_isr = s.address() + sizeof(Bootloader::KernelHeader) - sizeof(uint32_t) * 3 - 0x90000000;
  // we allocate a big buffer to store the first sector
  uint8_t data[1024*4];
  memcpy(data, (void*)0x90000000, 1024*4);
  uint32_t dummy_address = (uint32_t)&_fake_isr_function_start + 1;
  uint8_t * ptr = (uint8_t *)&dummy_address;
  data[origin_isr + sizeof(uint32_t) * 6] = ptr[0]; // BusFault
  data[origin_isr + sizeof(uint32_t) * 6 + 1] = ptr[1];
  data[origin_isr + sizeof(uint32_t) * 6 + 2] = ptr[2];
  data[origin_isr + sizeof(uint32_t) * 6 + 3] = ptr[3];

  // data[origin_isr + sizeof(uint32_t) * 5] = ptr[0]; // MemManage
  // data[origin_isr + sizeof(uint32_t) * 5 + 1] = ptr[1];
  // data[origin_isr + sizeof(uint32_t) * 5 + 2] = ptr[2];
  // data[origin_isr + sizeof(uint32_t) * 5 + 3] = ptr[3];
  
  data[origin_isr + sizeof(uint32_t) * 7] = ptr[0]; // UsageFault
  data[origin_isr + sizeof(uint32_t) * 7 + 1] = ptr[1];
  data[origin_isr + sizeof(uint32_t) * 7 + 2] = ptr[2];
  data[origin_isr + sizeof(uint32_t) * 7 + 3] = ptr[3];

  // data[origin_isr + sizeof(uint32_t) * 4] = ptr[0];//hardfault
  // data[origin_isr + sizeof(uint32_t) * 4 + 1] = ptr[1]; 
  // data[origin_isr + sizeof(uint32_t) * 4 + 2] = ptr[2];
  // data[origin_isr + sizeof(uint32_t) * 4 + 3] = ptr[3];

  Ion::Device::ExternalFlash::EraseSector(0);
  Ion::Device::ExternalFlash::WriteMemory((uint8_t*)0x90000000, data, 1024*4);
}

void Boot::bootSlot(Bootloader::Slot s) {
  config()->setSlot(&s);
  if (!s.userlandHeader()->isOmega() && !s.userlandHeader()->isUpsilon()) {
    // We are trying to boot epsilon, so we check the version and show an advertisement if needed
    const char * version = s.userlandHeader()->version();
    const char * min = "20.3.1";
    int versionSum = Utility::versionSum(version, strlen(version));
    int minimalVersionTrigger = Utility::versionSum(min, strlen(min));
    if (versionSum >= minimalVersionTrigger) {
      WarningMenu menu = WarningMenu();
      menu.open();
      return;
    }
  }
  bootSelectedSlot();
}

void Boot::bootSelectedSlot() {
  lockInternal();
  config()->setBooting(true);
  Ion::Device::Flash::LockInternalFlashForSession();
  config()->slot()->boot();
}

__attribute__((noreturn)) void Boot::boot() {
  assert(mode() != BootMode::Unknown);

  Boot::config()->clearSlot();
  Boot::config()->setBooting(false);

  while (true) {
      HomeMenu menu = HomeMenu();
      menu.open(true);
  }

  // Achievement unlocked: How Did We Get Here?
  bootloader();
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
        return;
      } else if (scan == Ion::Keyboard::State(Ion::Keyboard::Key::OnOff)) {
        Ion::Power::standby(); // Force a core reset to exit
      }
    } while (!Ion::USB::isEnumerated());

    // Launch the DFU stack, allowing to press Back to quit and reset
    Ion::USB::DFU(true, &data);
  }
}

void Boot::jumpToInternalBootloader() {
  Ion::Device::Board::jumpToInternalBootloader();
}

void Boot::lockInternal() {
  Ion::Device::Flash::DisableInternalProtection();
  Ion::Device::Flash::SetInternalSectorProtection(0, true);
  Ion::Device::Flash::SetInternalSectorProtection(1, true);
  Ion::Device::Flash::SetInternalSectorProtection(2, true);
  Ion::Device::Flash::SetInternalSectorProtection(3, true);
  Ion::Device::Flash::EnableInternalProtection();
}

void Boot::EnableInternalFlashInterrupt() {
  Ion::Device::Flash::EnableInternalFlashInterrupt();
}

}
