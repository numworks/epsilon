#include <shared/drivers/flash_write_with_interruptions.h>

#include "svcall.h"

namespace Ion {
namespace Device {
namespace Flash {

void SVC_ATTRIBUTES MassEraseWithInterruptions(bool handleInterrupts) {
  SVC_RETURNING_VOID(SVC_FLASH_MASS_ERASE_WITH_INTERRUPTIONS)
}

bool SVC_ATTRIBUTES EraseSectorWithInterruptions(int i, bool handleInterrupts) {
  SVC_RETURNING_R0(SVC_FLASH_ERASE_SECTOR_WITH_INTERRUPTIONS, bool)
}

bool SVC_ATTRIBUTES WriteMemoryWithInterruptions(uint8_t* destination,
                                                 const uint8_t* source,
                                                 size_t length,
                                                 bool handleInterrupts) {
  SVC_RETURNING_R0(SVC_FLASH_WRITE_MEMORY_WITH_INTERRUPTIONS, bool)
}

}  // namespace Flash
}  // namespace Device
}  // namespace Ion
