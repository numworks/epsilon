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

constexpr static size_t SizeSize = sizeof(uint32_t);
constexpr static size_t SignatureSize = 64;
/* This information could be extracted from the linking of the userland but we
 * want to be able to compile the kernel beforehand. We thus static assertion
 * to check that our hardcoded value is correct when compiling the userland */
constexpr static uint32_t UserlandHeaderOffset = 8;

}
}
}
}

#endif
