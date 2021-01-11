#ifndef ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_BOARD_H

#include <drivers/config/internal_flash.h>
#include <ion/src/device/shared/drivers/config/board.h>
#include <stdint.h>
#include <stddef.h>
#include <shared/drivers/config/persisting_bytes.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

/* The bootloader, kernel and userland starts should be aligned to the begining of a sector (to flash them easily).
 * The memory layouts are the following:
 * - internal flash: 4*16k
 * - external flash: 8*4k + 32K + 127 * 64K
 */

constexpr static uint32_t KernelInternalTotalSize = InternalFlash::Config::TotalSize - 2*BootloaderTotalSize;
constexpr static uint32_t KernelExternalTotalSize = 8*0x1000 + 0x8000 - PersistingBytes::Config::BufferSize;


}
}
}
}

#endif

