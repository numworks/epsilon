#ifndef ION_DEVICE_CORE_SHARED_FLASH_PRIVILEGED_H
#define ION_DEVICE_CORE_SHARED_FLASH_PRIVILEGED_H

#include <shared/drivers/flash_unprivileged.h>
#include <stddef.h>

namespace Ion {
namespace Device {
namespace Flash {

// Authorization
bool ForbiddenSector(int i);
bool MassEraseEnable();

// Actions
void MassErase();
bool EraseSector(int i);
bool WriteMemory(uint8_t * destination, const uint8_t * source, size_t length);

}
}
}

#endif
