#ifndef ION_DEVICE_SHARED_INTERNAL_FLASH_PRIVILEGED_H
#define ION_DEVICE_SHARED_INTERNAL_FLASH_PRIVILEGED_H

#include "internal_flash_unprivileged.h"
#include <stddef.h>

namespace Ion {
namespace Device {
namespace InternalFlash {

void EraseSector(int i);

void WriteMemory(uint8_t * destination, uint8_t * source, size_t length);

}
}
}

#endif
