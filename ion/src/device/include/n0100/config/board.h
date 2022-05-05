#ifndef ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_BOARD_H

#include <shared/boot/isr.h>
#include <config/internal_flash.h>
#include <stdint.h>
#include <stddef.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

/* The kernel and userland starts should be aligned to the begining of a sector
 * (to flash them easily).
 * The internal memory layout is the following:
 * 4*16k + 64k + 7*128k with the division:
 * +-------------+------------------+----------------+
 * |    2*16k    |        16k       + 16k+64k+7*128k |
 * +-------------+------------------+----------------+
 * |    Kernel   | Persisting bytes |    Userland    |
 * +-------------+------------------+----------------+
 * Hence the kernel total size.
 *
 */

constexpr static uint32_t KernelStartAddress = Config::InternalFlashOrigin;
constexpr static uint32_t KernelHeaderAddress = KernelStartAddress + INITIALISATION_VECTOR_SIZE * sizeof(uint32_t);
constexpr static uint32_t KernelSize = 2*0x4000; // 2*16k
constexpr static uint32_t PersistingBytesSize = 0x4000; // 16k
constexpr static uint32_t UserlandStartAddress = KernelStartAddress + KernelSize + PersistingBytesSize;

}
}
}
}

#endif


