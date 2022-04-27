#ifndef ION_DEVICE_SHARED_INTERNAL_FLASH_H
#define ION_DEVICE_SHARED_INTERNAL_FLASH_H

#include <stddef.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace InternalFlash {

void MassErase();

int SectorAtAddress(uint32_t address);
void EraseSector(int i);

void WriteMemory(uint8_t * destination, uint8_t * source, size_t length);

void EnableProtection();
void DisableProtection();
void SetSectorProtection(int i, bool protect);
void EnableSessionLock();
void EnableFlashInterrupt();
void ClearErrors();

/* The Device is powered by a 2.8V LDO. This allows us to perform writes to the
 * Flash 32 bits at once. */
constexpr Regs::FLASH::CR::PSIZE MemoryAccessWidth = Regs::FLASH::CR::PSIZE::X32;
typedef uint32_t MemoryAccessType;

}
}
}

#endif
