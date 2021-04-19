#ifndef ION_DEVICE_SHARED_DRIVERS_INTERNAL_FLASH_H
#define ION_DEVICE_SHARED_DRIVERS_INTERNAL_FLASH_H

#include <stddef.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace InternalFlash {

void MassErase();
void EraseSector(int i);
void WriteMemory(uint8_t * destination, const uint8_t * source, size_t length);

}
}
}

#endif
