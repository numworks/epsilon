#ifndef ION_DEVICE_N0115_CONFIG_INTERNAL_FLASH_H
#define ION_DEVICE_N0115_CONFIG_INTERNAL_FLASH_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace InternalFlash {
namespace Config {

constexpr int NumberOfSectors = 4;
constexpr uint32_t SectorAddresses[NumberOfSectors + 1] = {
    0x08000000, 0x08004000, 0x08008000, 0x0800C000, 0x08010000};

}  // namespace Config
}  // namespace InternalFlash
}  // namespace Device
}  // namespace Ion

#endif
