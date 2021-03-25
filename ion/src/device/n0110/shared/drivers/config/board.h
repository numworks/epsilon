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
 * The bootloader should occupty the whole internal flash
 * The memory layouts are the following:
 * - internal flash: 4*16k
 * - external flash: 8*4k + 32K + 127 * 64K
 */

constexpr static uint32_t BootloaderSize = InternalFlash::Config::TotalSize; // 64kB
constexpr static uint32_t KernelSize = 0x40000; // 256kB
constexpr static uint32_t UserlandOffsetFromKernel = KernelSize + SizeSize;

constexpr static uint32_t STBootloaderAddress = 0x00100000;
constexpr static uint32_t ITCMInterface = 0x00200000;
constexpr static uint32_t AXIMInterface = 0x08000000;

constexpr static uint32_t BootloaderStartAddress = ITCMInterface;
constexpr static uint32_t BootloaderSectionSize = InternalFlash::Config::TotalSize / InternalFlash::Config::NumberOfSectors;
constexpr static uint32_t BootloaderTrampolineAddress = BootloaderStartAddress + (InternalFlash::Config::NumberOfSectors - 1) * BootloaderSectionSize;

}
}
}
}

#endif

