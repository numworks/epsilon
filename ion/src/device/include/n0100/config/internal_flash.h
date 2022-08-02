#ifndef ION_DEVICE_N0100_CONFIG_INTERNAL_FLASH_H
#define ION_DEVICE_N0100_CONFIG_INTERNAL_FLASH_H

#include <stdint.h>

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

}
}
}
}

#endif
