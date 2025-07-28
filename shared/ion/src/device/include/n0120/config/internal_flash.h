#ifndef ION_DEVICE_N0120_CONFIG_INTERNAL_FLASH_H
#define ION_DEVICE_N0120_CONFIG_INTERNAL_FLASH_H

#include <stddef.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace InternalFlash {
namespace Config {

constexpr int NumberOfSectors = 4;
constexpr uint32_t SectorAddresses[NumberOfSectors + 1] = {
    0x08000000, 0x08020000, 0x08040000, 0x08060000, 0x08080000,
};

}  // namespace Config
}  // namespace InternalFlash
}  // namespace Device
}  // namespace Ion

#endif
