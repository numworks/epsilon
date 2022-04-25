#include <bootloader/slots/slot.h>
#include <ion/src/device/shared/drivers/board.h>
#include <ion/src/device/shared/drivers/flash.h>
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
  return m_userlandHeader;
}

[[ noreturn ]] void Slot::boot() const {

  if (m_address == 0x90000000) {
    // If we are booting from slot A, we need to lock the slot B
    Ion::Device::Flash::LockSlotB();
  } else {
    // If we are booting from slot B, we need to lock the slot A (and Khi)
    Ion::Device::Flash::LockSlotA();
  }

  // Configure the MPU for the booted firmware
  Ion::Device::Board::bootloaderMPU();

  // Jump
  jump_to_firmware(kernelHeader()->stackPointer(), kernelHeader()->startPointer());
  for(;;);
}

}
