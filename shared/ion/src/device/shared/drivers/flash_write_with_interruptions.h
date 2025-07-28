#ifndef ION_DEVICE_SHARED_FLASH_WRITE_WITH_INTERRUPTIONS_H
#define ION_DEVICE_SHARED_FLASH_WRITE_WITH_INTERRUPTIONS_H

#include <stddef.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace Flash {

void MassEraseWithInterruptions(bool handleInterrupts);
bool EraseSectorWithInterruptions(int i, bool handleInterrupts);
bool WriteMemoryWithInterruptions(uint8_t* destination, const uint8_t* source,
                                  size_t length, bool handleInterrupts);

}  // namespace Flash
}  // namespace Device
}  // namespace Ion

#endif
