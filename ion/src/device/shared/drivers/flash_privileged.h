#ifndef ION_DEVICE_SHARED_FLASH_PRIVILEGED_H
#define ION_DEVICE_SHARED_FLASH_PRIVILEGED_H

#include "flash_unprivileged.h"
#include <stddef.h>

namespace Ion {
namespace Device {
namespace Flash {

bool ForbiddenSector(int i);
bool MassEraseEnable();
void MassErase();
bool EraseSector(int i);
bool WriteMemory(uint8_t * destination, const uint8_t * source, size_t length);

}
}
}

#endif
