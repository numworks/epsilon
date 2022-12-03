#include <bootloader/slots/slot.h>
#include <ion/src/device/shared/drivers/board.h>
#include <ion/src/device/shared/drivers/flash.h>
#include <ion/src/device/shared/drivers/external_flash.h>
#include <bootloader/boot.h>

extern "C" void jump_to_firmware(const uint32_t* stackPtr, const void(*startPtr)(void));

namespace Bootloader {

const Slot Slot::A() {
  return Slot(0x90000000);
}

const Slot Slot::B() {
  return Slot(0x90400000);
}

const Slot Slot::Khi() {
  return Slot(0x90180000);
}

const KernelHeader* Slot::kernelHeader() const {
  return m_kernelHeader;
}

const UserlandHeader* Slot::userlandHeader() const {
  if (m_userlandHeader->isValid()) {
    return m_userlandHeader;
  } else if (m_userland2Header->isValid()) {
    return m_userland2Header;
  } else {
    return m_userlandHeader;
  }
}

[[ noreturn ]] void Slot::boot() const {

  if (m_address == 0x90000000) {
    // If we are booting from slot A, we need to lock the slot B
    Ion::Device::Flash::LockSlotB();
  } else {
    // If we are booting from slot B, we need to lock the slot A (and Khi)
    Ion::Device::Flash::LockSlotA();
  }

  // Erase the bootloader integrated in slots in Epsilon 20
  if (m_userland2Header->isValid()) {
    if (m_address == 0x90000000) {
      // Check if bootloader is present in slot A
      if (*(uint32_t*)0x90010000 != 0xFFFFFFFF) {
        // Erase bootloader in slot A
        Ion::Device::ExternalFlash::EraseSector(9);
      }
    }
    else if (m_address == 0x90400000) {
      // Check if bootloader is present in slot B
      if (*(uint32_t*)0x90410000 != 0xFFFFFFFF) {
        // Erase bootloader in slot B
        Ion::Device::ExternalFlash::EraseSector(73);
      }
    }
  }

  // Configure the MPU for the booted firmware
  Ion::Device::Board::bootloaderMPU();

  // Jump
  jump_to_firmware(kernelHeader()->stackPointer(), kernelHeader()->startPointer());
  for(;;);
}

}
