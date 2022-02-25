#include <bootloader/slot.h>

namespace Bootloader {

  const struct Slot* s_slotA = reinterpret_cast<const struct Slot*>(0x90000000);
  const struct Slot* s_slotB = reinterpret_cast<const struct Slot*>(0x90400000);

}
