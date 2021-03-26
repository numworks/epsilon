#ifndef ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_BOARD_H

#include <drivers/config/internal_flash.h>
#include <ion/src/device/shared/drivers/config/board.h>
#include <stdint.h>
#include <stddef.h>
#include <shared/drivers/config/persisting_bytes.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

/* The kernel and userland starts should be aligned to the begining of a sector
 *(to flash them easily).
 * The internal memory layout is the following:
 * 4*16k + 64k + 7*128k with the division:
 * +---------------------+-------------------+-----------+--------------|
 * |         16k         +         16k       + 2*16k+64k +  64k+7*128k  |
 * +---------------------+-------------------+-----------+--------------|
 * | Standard bootloader |  Persisting bytes |  Kernel   |   Userland   |
 * +---------------------+-------------------+-----------+--------------|
 * Hence the kernel total size.
 *
 * NB: Total size includes signature footer (unlike length)
 */

constexpr static uint32_t BootloaderSize = 0x4000; // 64kB
constexpr static uint32_t STBootloaderAddress = 0x1FFF0000;
constexpr static uint32_t BootloaderStartAddress = InternalFlash::Config::StartAddress;

constexpr static uint32_t KernelSize = 2*0x4000 + 0x10000; // 2*16k + 64
constexpr static uint32_t UserlandOffsetFromKernel = KernelSize + SizeSize;
constexpr static uint32_t BootloaderTrampolineAddress = //TODO;
constexpr static uint32_t KernelAStartAddress = InternalFlash::Config::StartAddress + BootloaderTotalSize + PersistingBytes::Config::BufferSize;
// N0100 kernel has no duplicate
constexpr static uint32_t KernelBStartAddress = KernelAStartAddress;
constexpr static uint32_t KernelLength = KernelSize - Board::Config::SignatureSize;

}
}
}
}

#endif


