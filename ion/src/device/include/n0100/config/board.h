#ifndef ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_BOARD_H

#include <stdint.h>

/* This file is used to generate the corresponding .ld file for the linker.
 * Declarations must be of the form:
 *   constexpr <integer type> <name> = <expression>; [// <comment>]
 * Comments and blank lines are ignored.
 * Macros such as "#if" may be used, as the preprocessor will be run before
 * this file is converted to .ld.
 * Calls to "static_assert" are replaced with LD "ASSERT". */

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

/* Internal flash
 * |                     INTERNAL FLASH                        |
 * | 16K + 16K + 16k |           16K          |  64k + 7*128K  |
 * |      KERNEL     | PERSISTIG BYTES BUFFER |   USERLAND     |
 *
 */

constexpr uint32_t InternalFlashOrigin = 0x08000000;
constexpr uint32_t InternalFlashLength = 0x100000; // 1MiB
constexpr uint32_t SmallestFlashSectorLength = 0x4000; // 16KiB

constexpr uint32_t KernelLength = 2 * SmallestFlashSectorLength;
constexpr uint32_t KernelVirtualOrigin = InternalFlashOrigin;

constexpr uint32_t ExtraDataLength = 0;
constexpr uint32_t ExtraDataVirtualOrigin = 0;

constexpr uint32_t PersistingBytesLength = SmallestFlashSectorLength;
constexpr uint32_t PersistingBytesVirtualOrigin = KernelVirtualOrigin + KernelLength;

constexpr uint32_t UserlandLength = InternalFlashLength - KernelLength - PersistingBytesLength;
constexpr uint32_t UserlandVirtualOrigin = PersistingBytesVirtualOrigin + PersistingBytesLength;

/* SRAM */
constexpr uint32_t SRAMOrigin = 0x20000000;
constexpr uint32_t SRAMLength = 0x40000; // 256KiB

constexpr uint32_t KernelStackLength = 0x400; // 1KiB
constexpr uint32_t KernelDataBSSLength = 0xC00; // 3KiB
constexpr uint32_t KernelSRAMLength = KernelStackLength + KernelDataBSSLength;
constexpr uint32_t KernelSRAMOrigin = SRAMOrigin + SRAMLength - KernelSRAMLength;

constexpr uint32_t UserlandStackLength = 0x8000; // 32KiB
constexpr uint32_t UserlandSRAMOrigin = SRAMOrigin;
constexpr uint32_t UserlandSRAMLength = SRAMLength - KernelSRAMLength;

constexpr uint32_t STBootloaderStack = 0x8000; // 32KiB

constexpr uint32_t FlasherLength = 0x10000; // 64KiB
constexpr uint32_t FlasherOffset = SRAMLength - FlasherLength;
static_assert(STBootloaderStack < FlasherOffset, "The flasher overlaps ST bootloader stack.");

constexpr uint32_t BenchLength = 0x20000; // 128KiB
constexpr uint32_t BenchOffset = 0;

/* Signature */
constexpr uint32_t SignedPayloadLength = 0;
constexpr uint32_t CanaryLength = 0;

constexpr uint32_t SignatureLength = 0;

}
}
}
}

#endif
