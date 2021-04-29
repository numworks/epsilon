#ifndef ION_DEVICE_N0110_CONFIG_INTERNAL_FLASH_H
#define ION_DEVICE_N0110_CONFIG_INTERNAL_FLASH_H

#include <ion/src/device/shared/drivers/config/internal_flash.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace InternalFlash {
namespace Config {

constexpr static int NumberOfForbiddenFirstSectors = 2;
constexpr static uint32_t StartAddress = 0x08000000;
constexpr static uint32_t EndAddress = 0x08010000;
constexpr static uint32_t TotalSize = EndAddress - StartAddress;
constexpr static int NumberOfSectors = 4;
constexpr static uint32_t SectorAddresses[NumberOfSectors+1] = {
  0x08000000, 0x08004000, 0x08008000, 0x0800C000,
  0x08010000
};

}
}
}
}

#endif

