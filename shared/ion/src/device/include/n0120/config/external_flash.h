#ifndef ION_DEVICE_N0120_CONFIG_EXTERNAL_FLASH_H
#define ION_DEVICE_N0120_CONFIG_EXTERNAL_FLASH_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {
namespace Config {

constexpr int NumberOf4KSectors = 8;
constexpr int NumberOf32KSectors = 1;
constexpr int NumberOf64KSectors = 128 - 1;
constexpr int NumberOfSectors =
    NumberOf4KSectors + NumberOf32KSectors + NumberOf64KSectors;

constexpr uint8_t NumberOfAddressBitsIn64KbyteBlock = 16;
constexpr uint8_t NumberOfAddressBitsIn32KbyteBlock = 15;
constexpr uint8_t NumberOfAddressBitsIn4KbyteBlock = 12;

}  // namespace Config
}  // namespace ExternalFlash
}  // namespace Device
}  // namespace Ion

#endif
