#include <bootloader/slot.h>

extern "C" void jump_to_firmware(const uint32_t* stackPtr, const void(*startPtr)(void));

namespace Bootloader {

const Slot Slot::A() {
  return Slot(0x90000000);
}

const Slot Slot::B() {
  return Slot(0x90400000);
}

const KernelHeader* Slot::kernelHeader() const {
  return m_kernelHeader;
}

const UserlandHeader* Slot::userlandHeader() const {
  return m_userlandHeader;
}

[[ noreturn ]] void Slot::boot() const {
  jump_to_firmware(kernelHeader()->stackPointer(), kernelHeader()->startPointer());
  for(;;);
}

}