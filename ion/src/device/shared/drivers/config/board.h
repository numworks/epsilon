#ifndef ION_DEVICE_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_SHARED_DRIVERS_CONFIG_BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <drivers/hash.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

/* The bootloader, kernel and userland starts should be aligned to the begining of a sector (to flash them easily).
 * The internal memory layouts are the following:
 * - N0100: 4*16k + 64k + 7*128k
 * - N0110: 4*16k
 * Hence the bootloader total size.
 *
 * NB: Total size includes size header and signature footer (unlike length)
 */

constexpr static uint32_t BootloaderTotalSize = 0x4000;
constexpr static size_t SizeSize = sizeof(uint32_t);
constexpr static size_t SignatureSize = Hash::Sha256DigestBytes;


}
}
}
}

#endif
