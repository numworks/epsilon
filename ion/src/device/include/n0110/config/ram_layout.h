#ifndef ION_DEVICE_INCLUDE_N0110_CONFIG_BOARD_H
#define ION_DEVICE_INCLUDE_N0110_CONFIG_BOARD_H

#include <stdint.h>
#include <stddef.h>

namespace Ion {
namespace Device {
namespace Board {

// RAM
constexpr static uint32_t SRAMAddress = 0x20000000;
constexpr static uint32_t SRAMLength = 0x40000; // 256kB

}
}
}

#endif

