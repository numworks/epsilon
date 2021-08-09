#ifndef ION_DEVICE_N0110_USERLAND_DRIVERS_CONFIG_BOARD_RAM_H
#define ION_DEVICE_N0110_USERLAND_DRIVERS_CONFIG_BOARD_RAM_H

#include <shared/drivers/config/board.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

/* N0110 userland DFU cannot write the SRAM allocated for the kernel. */

constexpr static uint32_t WritableSRAMStartAddress = UserlandSRAMAddress;
constexpr static uint32_t WritableSRAMEndAddress = UserlandSRAMAddress + UserlandSRAMLength;

}
}
}
}

#endif
