#ifndef ION_DEVICE_N0100_CONFIG_INTERNAL_FLASH_H
#define ION_DEVICE_N0100_CONFIG_INTERNAL_FLASH_H

#include <config/board.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace InternalFlash {
namespace Config {

constexpr static uint32_t StartAddress = 0x08000000;
constexpr static uint32_t EndAddress = 0x08100000;
constexpr static uint32_t TotalSize = EndAddress - StartAddress;
constexpr static int NumberOfSectors = 12;
constexpr static uint32_t SectorAddresses[NumberOfSectors+1] = {
  0x08000000, 0x08004000, 0x08008000, 0x0800C000,
  0x08010000, 0x08020000, 0x08040000, 0x08060000,
  0x08080000, 0x080A0000, 0x080C0000, 0x080E0000,
  0x08100000
};

/* The Device is powered by a 2.8V LDO. This allows us to perform writes to the
 * Flash 32 bits at once. */
constexpr Regs::FLASH::CR::PSIZE MemoryAccessWidth = Regs::FLASH::CR::PSIZE::X32;
typedef uint32_t MemoryAccessType;

}
}
}
}

#endif
