#ifndef ION_DEVICE_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_SHARED_DRIVERS_CONFIG_BOARD_H

#include <stdint.h>
#include <stddef.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

/* The bootloader, kernel and userland starts should be aligned to the begining of a sector (to flash them easily).
 * The memory layouts are the following:
 * - N0100: 4*16k + 64k + 7*128k
 * - N0110: 4*16k (internal) | 8*4k + 32k + 127 * 64k (external)
 *
 * NB: Total size includes size header and signature footer (unlike length)
 */

constexpr static size_t SignedPayloadSize = 8;
constexpr static size_t SignatureSize = 64;

// RAM
constexpr static uint32_t SRAMAddress = 0x20000000;
constexpr static uint32_t SRAMLength = 0x40000; // 256kB

constexpr static uint32_t ITCMRAMAdress = 0x00000000; // 16kB

// Kernal RAM
constexpr static uint32_t KernelSRAMDataBSSLength = 0xC00; // 3k
constexpr static uint32_t KernelStackLength = 0x400; // 1K
constexpr static uint32_t KernelRAMAddress = SRAMAddress + SRAMLength - KernelSRAMDataBSSLength - KernelStackLength;

// Userland RAm
constexpr static uint32_t UserlandSRAMAddress = SRAMAddress;
constexpr static uint32_t UserlandSRAMLength = SRAMLength - KernelSRAMDataBSSLength - KernelStackLength;
constexpr static uint32_t UserlandStackLength = 0x8000; // 32K

}
}
}
}

#endif
