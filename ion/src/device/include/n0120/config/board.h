#ifndef ION_DEVICE_N0120_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_N0120_SHARED_DRIVERS_CONFIG_BOARD_H

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

/* The bootloader, kernel and userland starts should be aligned to the begining of a sector (to flash them easily).
 * The bootloader should occupy the whole internal flash
 * The memory layouts are the following:
 * - internal flash: 4*128K
 * - external flash: 8*4k + 32K + 127 * 64K
 */

/* Internal flash */
constexpr uint32_t InternalFlashOrigin = 0x08000000;
constexpr uint32_t InternalFlashLength = 0x80000; // 512KiB

constexpr uint32_t STBootloaderAddress = 0x1FF00000;

constexpr uint32_t BootloaderOrigin = InternalFlashOrigin;
constexpr uint32_t PseudoOTPLength = 512;
constexpr uint32_t TrampolineLength = 0x400; // 1KiB
constexpr uint32_t BootloaderLength = InternalFlashLength - PseudoOTPLength - TrampolineLength;
constexpr uint32_t PseudoOTPOrigin = BootloaderOrigin + BootloaderLength;
constexpr uint32_t TrampolineOrigin = PseudoOTPOrigin + PseudoOTPLength;

// clang-format off
/* External flash
 *
 * ||                                                    EXTERNAL fLASH (8mIb)                                                         ||
 * ||         64K          |      64K or 0     |              61~62 * 64K               |       64K        ||         64 * 64K         ||
 * ||                           SIGNED PAYLOAD A                        | EXTERNAL APPS | PERSISTING BYTES || ... Identical B Slot ... ||
 * || SP HEADER | KERNEL A |     EXTRA DATA    | USERLAND A | SIGNATURE |       "       |        "         ||                          ||
 *
 * - SP HEADER: || SECURITY LEVEL | PAYLOAD LENGTH (WITHOUT HEADER) ||
 * - KERNEL:    || HEADER | INIT VECTOR | CODE | 0b111...1 ||
 * - USERLAND:  || HEADER | INIT VECTOR | CODE ||
 *
 * Kernel and Userland are prefixed by:
 * - a header listing various information that have to be retrieved from DFU
 * - a initialisation vector starting with the entry point and the stack initialization
 *
 * The kernel initialisation vector table has specific requirements regarding:
 * - its memory space: it must be in the range 0x00000080 to 0x3FFFFF80
 * - its alignement: you must align the offset to the number of exception entries in the vector table. The minimum alignment is 128 words.
 * [https://www.st.com/content/ccc/resource/technical/document/programming_manual/6c/3a/cb/e7/e4/ea/44/9b/DM00046982.pdf/files/DM00046982.pdf/jcr:content/translations/en.DM00046982.pdf]
 * Therefore, its is also relocated in sRAM at booting.
 */
// clang-format on

constexpr uint32_t ExternalFlashOrigin = 0x90000000;
constexpr uint32_t ExternalFlashLength = 0x800000; // 8MiB
constexpr uint32_t StandardExternalFlashSectorLength = 0x10000; // 64KiB

constexpr uint32_t SlotAOffset = 0;
constexpr uint32_t SlotAOrigin = ExternalFlashOrigin + SlotAOffset;
constexpr uint32_t SlotBOffset = ExternalFlashLength / 2;
constexpr uint32_t SlotBOrigin = ExternalFlashOrigin + SlotBOffset;

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

constexpr uint32_t ExternalAppsSectorUnit = 0x10000;

/* SRAM
 *
 * |                                SRAM                                                 |
 * | USERLAND DATA/BSS | HEAP | USERLAND STACK | KERNEL STACK | KERNEL DATA/BSS - canary |
 *
 * Bootloader SRAM is mapped like:
 * |                                    STACK                                   | canary |
 * Kernel SRAM is mapped like:
 * | xxxxxxxxxxxxxxxxxxxxxxxx | USERLAND STACK | KERNEL STACK | KERNEL DATA/BSS - canary |
 * Userland SRAM is mapped like:
 * | USERLAND DATA/BSS | HEAP | USERLAND STACK | xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx |
 *
 * The few restrictions to layout the RAM memory are the following:
 * - the begining of Kernel data/bss section should be aligned: the isr table
 * is within this section and the VTOR registers can only be configure with
 * some alignments constraints.
 * - the kernel RAM should be aligned to be able to add MPU protection over
 * this region: MPU region must be aligned to the size of the region.
 * - Overflowing the userland stack should not impact the kernel (the MPU region
 * should protect it anyway).
 */

constexpr uint32_t SRAMOrigin = 0x24000000;
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

constexpr uint32_t BackupSRAMOrigin = 0x38800000;
constexpr uint32_t BackupSRAMLength = 0x1000; // 4kiB

/* Signature */
constexpr uint32_t SignedPayloadLength = 8;
constexpr uint32_t CanaryLength = 4;

constexpr uint32_t SingleSignatureLength = 64;
constexpr uint32_t NumberOfSignatures = 16;
constexpr uint32_t SignatureLength = SingleSignatureLength * NumberOfSignatures;

/* Other */
constexpr int NumberOfMPUSectors = 8;

}
}
}
}

#endif

