#ifndef ION_DEVICE_SHARED_EXTERNAL_FLASH_H
#define ION_DEVICE_SHARED_EXTERNAL_FLASH_H

#include <stddef.h>
#include <stdint.h>

// Quad-SPI on STM32 microcontroller
// https://www.st.com/resource/en/application_note/dm00227538.pdf

// Adesto Technologies AT25SF641
// https://www.adestotech.com/wp-content/uploads/AT25SF641_111.pdf

/* External Flash Memory map             Address space
 *                       8MiB chip     0x000000 - 0x7FFFFF
 *   2^7                64KiB blocks   0x..0000 - 0x..FFFF
 *   2^7 * 2            32KiB blocks   0x..0000 - 0x..7FFF or 0x..8000 - 0x..FFFF
 *   2^7 * 2 * 2^3       4KiB blocks   0x...000 - 0x...FFF
 *   2^7 * 2 * 2^3 * 2^4 256B pages    0x....00 - 0x....FF
 *
 * To be able to erase a small sector for the exam mode, we say that the flash
 * is cut into 8 + 1 + 2^7-1 = 136 sectors: 8 sectors of 4Kb, 1 sector of 32Kb
 * and 2^7-1 sectors of 64Kb. These sectors are the smallest erasable units. If
 * need be, we can define more sectors to erase even more finely the flash. */

namespace Ion {
namespace Device {
namespace ExternalFlash {

void init();
void shutdown();

void MassErase();
int SectorAtAddress(uint32_t address);
void EraseSector(int i);
void WriteMemory(uint8_t * destination, const uint8_t * source, size_t length);
void JDECid(uint8_t * manufacturerID, uint8_t * memoryType, uint8_t * capacityType);

void LockSlotA();
void LockSlotB();

static constexpr uint8_t NumberOfAddressBitsInChip = 23; // 2^23 Bytes = 8 MBytes
static constexpr uint32_t FlashAddressSpaceSize = 1 << NumberOfAddressBitsInChip;

}
}
}

#endif
