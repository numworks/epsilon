#include <bootloader/slot.h>

extern "C" void jump_to_firmware(const uint32_t* stackPtr, const void(*startPtr)(void));

namespace Bootloader {


const Slot* s_slotA = reinterpret_cast<const struct Slot*>(0x90000000);
const Slot* s_slotB = reinterpret_cast<const struct Slot*>(0x90400000);

const char * Slot::version() const {
  return m_version;
}

const char * Slot::patchLevel() const {
  return m_patchLevel;
}

const bool Slot::isValid() const {
  return m_header == Magic && m_footer == Magic;
}

[[ noreturn ]] void Slot::boot() const {
  jump_to_firmware(m_stackPointer, m_startPointer);
  for(;;);
}


}
