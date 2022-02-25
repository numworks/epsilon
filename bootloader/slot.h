#ifndef BOOTLOADER_SLOT
#define BOOTLOADER_SLOT

#include <stdint.h>

namespace Bootloader {

class Slot {
public:
  const char * version() const;
  const char * patchLevel() const;
  const bool isValid() const;
  [[ noreturn ]] void boot() const;

private:
  Slot();
  constexpr static uint32_t Magic = 0xDEC00DF0;
  const uint32_t m_unknown;
  const uint32_t m_signature;
  const uint32_t m_header;
  const char m_version[8];
  const char m_patchLevel[8];
  const uint32_t m_footer;
  const uint32_t* m_stackPointer;
  const void(*m_startPointer)();
};

extern const Slot* s_slotA;
extern const Slot* s_slotB;

}

#endif
