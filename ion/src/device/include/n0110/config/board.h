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

// Bootloader
constexpr uint32_t BootloaderSize = InternalFlash::Config::TotalSize; // 64kB
constexpr uint32_t STBootloaderAddress = 0x00100000;
constexpr uint32_t ITCMInterface = 0x00200000;
constexpr uint32_t AXIMInterface = 0x08000000;
constexpr uint32_t BootloaderStartAddress = ITCMInterface;
constexpr uint32_t BootloaderSectionSize = InternalFlash::Config::TotalSize / InternalFlash::Config::NumberOfSectors;
/* Trampoline could be smaller, but its size and position cannot be changed.
 * Otherwise, newer kernels would become would lose compatibility with older
 * bootloaders. */
constexpr uint32_t BootloaderTrampolineSize = 0x2000; // 8k
constexpr uint32_t BootloaderTrampolineAddress = BootloaderStartAddress + InternalFlash::Config::NumberOfSectors * BootloaderSectionSize - BootloaderTrampolineSize;

// Slots
constexpr uint32_t SlotAStartAddress = ExternalFlash::Config::StartAddress;
constexpr uint32_t SlotBStartAddress = ExternalFlash::Config::StartAddress + ExternalFlash::Config::TotalSize/2;

// Kernel
constexpr uint32_t KernelSize = 0x10000; // 64kB

// Bootloader update
constexpr uint32_t BootloaderUpdateSize = ExternalFlash::Config::MaximalSectorSize;

// Userland
constexpr uint32_t UserlandOffset = KernelSize + BootloaderUpdateSize;

// External apps
constexpr uint32_t ExternalAppsSectorUnit = 0x10000;

// MPU
constexpr int NumberOfMPUSectors = 8;

}
}
}
}

#endif

