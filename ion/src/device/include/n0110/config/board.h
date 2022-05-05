#ifndef ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_BOARD_H

#include <config/internal_flash.h>
#include <config/external_flash.h>
#include <stdint.h>
#include <stddef.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

/* The bootloader, kernel and userland starts should be aligned to the begining of a sector (to flash them easily).
 * The bootloader should occupty the whole internal flash
 * The memory layouts are the following:
 * - internal flash: 4*16k
 * - external flash: 8*4k + 32K + 127 * 64K
 */

constexpr uint32_t STBootloaderAddress = 0x00100000;
constexpr uint32_t ExternalAppsSectorUnit = 0x10000;
constexpr int NumberOfMPUSectors = 8;

constexpr uint32_t SlotAOrigin = Device::Config::ExternalFlashOrigin;
constexpr uint32_t SlotBOrigin = Device::Config::ExternalFlashOrigin + Device::Config::ExternalFlashLength / 2;

}
}
}
}

#endif

