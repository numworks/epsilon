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
 * The bootloader should occupy the whole internal flash
 * The memory layouts are the following:
 * - internal flash: 4*16k
 * - external flash: 8*4k + 32K + 127 * 64K
 */

// Bootloader
constexpr static uint32_t BootloaderSize = InternalFlash::Config::TotalSize; // 1MiB
constexpr static uint32_t STBootloaderAddress = 0x1FF00000;
constexpr static uint32_t AXIMInterface = 0x08000000;
constexpr static uint32_t BootloaderStartAddress = AXIMInterface;
constexpr static uint32_t BootloaderSectionSize = InternalFlash::Config::TotalSize / InternalFlash::Config::NumberOfSectors;
constexpr static uint32_t BootloaderTrampolineSize = 0x2000; // 8k
constexpr static uint32_t BootloaderTrampolineAddress = BootloaderStartAddress + InternalFlash::Config::NumberOfSectors * BootloaderSectionSize - BootloaderTrampolineSize;

// Slots
constexpr static uint32_t SlotAStartAddress = ExternalFlash::Config::StartAddress;
constexpr static uint32_t SlotBStartAddress = ExternalFlash::Config::StartAddress + ExternalFlash::Config::TotalSize/2;

// Kernel
constexpr static uint32_t KernelSize = 0x10000; // 64kiB

// Userland
constexpr static uint32_t UserlandOffset = KernelSize;

// External apps
constexpr static uint32_t ExternalAppsSectorUnit = 0x10000;

}
}
}
}

#endif

