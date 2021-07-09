#ifndef ION_DEVICE_N0110_SHARED_EXTERNAL_FLASH_H
#define ION_DEVICE_N0110_SHARED_EXTERNAL_FLASH_H

#include <stddef.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

void MassErase();
void EraseSector(int i);
void WriteMemory(uint8_t * destination, const uint8_t * source, size_t length);

}
}
}

#endif
