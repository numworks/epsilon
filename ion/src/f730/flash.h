#ifndef ION_DEVICE_FLASH_H
#define ION_DEVICE_FLASH_H

#include <stddef.h>
#include "regs/flash.h"

namespace Ion {
namespace Flash {
namespace Device {

void MassErase();

constexpr int NumberOfSectors = 12;
int SectorAtAddress(uint32_t address);
void EraseSector(int i);

void WriteMemory(uint8_t * source, uint8_t * destination, size_t length);

/* The Device is powered by a 2.8V LDO. This allows us to perform writes to the
 * Flash 32 bits at once. */
constexpr FLASH::CR::PSIZE MemoryAccessWidth = FLASH::CR::PSIZE::X32;
typedef uint32_t MemoryAccessType;

}
}
}

#endif
