#ifndef ION_DEVICE_N0110_CONFIG_MEMORY_H
#define ION_DEVICE_N0110_CONFIG_MEMORY_H

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
namespace Config {

/* Internal flash */
constexpr uint32_t ITCMInterface = 0x00200000;
constexpr uint32_t AXIMInterface = 0x08000000;

constexpr uint32_t InternalFlashOrigin = AXIMInterface;
constexpr uint32_t InternalFlashLength = 0x10000; // 64KiB

constexpr uint32_t TrampolineLength = 0x2000; // 8KiB
constexpr uint32_t BootloaderLength = InternalFlashLength - TrampolineLength;
constexpr uint32_t BootloaderOrigin = ITCMInterface;
constexpr uint32_t TrampolineOrigin = BootloaderOrigin + BootloaderLength;

/* External flash */
constexpr uint32_t ExternalFlashOrigin = 0x90000000;
constexpr uint32_t ExternalFlashLength = 0x800000; // 8MiB
constexpr uint32_t StandardExternalFlashSectorLength = 0x10000; // 64KiB

constexpr uint32_t SlotAOffset = 0;
constexpr uint32_t SlotBOffset = ExternalFlashLength / 2;

constexpr uint32_t PersistingBytesLength = StandardExternalFlashSectorLength;
constexpr uint32_t PersistingBytesVirtualOrigin = ExternalFlashOrigin + ExternalFlashLength / 2 - PersistingBytesLength;

constexpr uint32_t KernelLength = StandardExternalFlashSectorLength;
constexpr uint32_t KernelVirtualOrigin = ExternalFlashOrigin;

#if EMBED_EXTRA_DATA
constexpr uint32_t ExtraDataLength = StandardExternalFlashSectorLength;
#else
constexpr uint32_t ExtraDataLength = 0;
#endif
constexpr uint32_t ExtraDataVirtualOrigin = KernelVirtualOrigin + KernelLength;

constexpr uint32_t UserlandLength = ExternalFlashLength / 2 - KernelLength - ExtraDataLength - PersistingBytesLength;
constexpr uint32_t UserlandVirtualOrigin = ExtraDataVirtualOrigin + ExtraDataLength;

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
constexpr uint32_t SignedPayloadLength = 8;
constexpr uint32_t CanaryLength = 4;

constexpr uint32_t SingleSignatureLength = 64;
constexpr uint32_t NumberOfSignatures = 16;
constexpr uint32_t SignatureLength = SingleSignatureLength * NumberOfSignatures;

}
}
}

#endif
