#ifndef ION_DEVICE_SHARED_DRIVERS_FLASH_PRIVILEGED_H
#define ION_DEVICE_SHARED_DRIVERS_FLASH_PRIVILEGED_H

#include "flash_unprivileged.h"
#include <stddef.h>

namespace Ion {
namespace Device {
namespace Flash {

void MassErase();
void EraseSector(int i);
void WriteMemory(uint8_t * destination, uint8_t * source, size_t length);

}
}
}

#endif
