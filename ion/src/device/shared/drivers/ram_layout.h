#ifndef ION_DEVICE_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_SHARED_DRIVERS_CONFIG_BOARD_H

#include <config/ram_layout.h>

namespace Ion {
namespace Device {
namespace Board {

/* The bootloader, kernel and userland starts should be aligned to the begining of a sector (to flash them easily).
 * The memory layouts are the following:
 * - N0100: 4*16k + 64k + 7*128k
 * - N0110: 4*16k (internal) | 8*4k + 32k + 127 * 64k (external)
 *
 * NB: Total size includes size header and signature footer (unlike length)
 */

constexpr static size_t SignedPayloadSize = 8;
constexpr static size_t SignatureSize = 64;

// Kernel RAM
constexpr static uint32_t KernelSRAMDataBSSLength = 0xC00; // 3k
constexpr static uint32_t KernelStackLength = 0x400; // 1K
constexpr static uint32_t KernelSRAMLength = KernelStackLength + KernelSRAMDataBSSLength;
constexpr static uint32_t KernelRAMAddress = SRAMAddress + SRAMLength - KernelSRAMLength;

// Userland RAm
constexpr static uint32_t UserlandSRAMAddress = SRAMAddress;
constexpr static uint32_t UserlandSRAMLength = SRAMLength - KernelSRAMLength;
constexpr static uint32_t UserlandStackLength = 0x8000; // 32K

// Programs executed in RAM
constexpr uint32_t BenchLength = 0x20000; // 128KiB
constexpr uint32_t FlasherLength = 0x10000; // 64KiB

}
}
}

#endif
