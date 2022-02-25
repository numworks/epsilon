#ifndef BOOTLOADER_SLOT
#define BOOTLOADER_SLOT

#include <stdint.h>

namespace Bootloader {

struct Slot {
  uint32_t unknown;
  uint32_t signature_offset;
  uint32_t magik_header;
  char version[8];
  char patch_level[8];
  uint32_t magik_footer;
  uint32_t* stack_pointer;
  void(*main_pointer)();
};

extern const struct Slot* s_slotA;
extern const struct Slot* s_slotB;

}


#endif