#ifndef ION_DEVICE_SHARED_FLASH_H
#define ION_DEVICE_SHARED_FLASH_H

#include <stddef.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace Flash {

int TotalNumberOfSectors();
int SectorAtAddress(uint32_t address);

void MassErase();
void EraseSector(int i);
void WriteMemory(uint8_t * destination, uint8_t * source, size_t length);

}
}
}

#endif
