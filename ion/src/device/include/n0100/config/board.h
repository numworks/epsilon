#ifndef ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_BOARD_H

#include <shared/boot/isr.h>
#include <config/memory_layout.h>
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

}
}
}
}

#endif
