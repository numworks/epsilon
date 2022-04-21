#ifndef ION_DEVICE_N0120_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_N0120_SHARED_DRIVERS_CONFIG_BOARD_H

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
 * - internal flash: 8*128K
 * - external flash: 8*4k + 32K + 127 * 64K
 */

// Bootloader
constexpr uint32_t BootloaderSize = InternalFlash::Config::TotalSize; // 512KiB
constexpr uint32_t STBootloaderAddress = 0x1FF00000;
constexpr uint32_t AXIMInterface = InternalFlash::Config::StartAddress;
constexpr uint32_t BootloaderStartAddress = AXIMInterface;
constexpr uint32_t BootloaderSectionSize = InternalFlash::Config::TotalSize / InternalFlash::Config::NumberOfSectors;
constexpr uint32_t BootloaderTrampolineSize = 0x400; // 1k
constexpr uint32_t BootloaderTrampolineAddress = BootloaderStartAddress + InternalFlash::Config::TotalSize - BootloaderTrampolineSize;
static_assert(BootloaderTrampolineAddress + BootloaderTrampolineSize == InternalFlash::Config::EndAddress, "Bootloader auxiliary section (pseudo-OTP + trampoline) are ill-mapped.");

// Slots
constexpr uint32_t SlotAStartAddress = ExternalFlash::Config::StartAddress;
constexpr uint32_t SlotBStartAddress = ExternalFlash::Config::StartAddress + ExternalFlash::Config::TotalSize/2;

// Kernel
constexpr uint32_t KernelSize = 0x10000; // 64kiB

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

