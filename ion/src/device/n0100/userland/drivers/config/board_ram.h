#ifndef ION_DEVICE_N0100_USERLAND_DRIVERS_CONFIG_BOARD_RAM_H
#define ION_DEVICE_N0100_USERLAND_DRIVERS_CONFIG_BOARD_RAM_H

#include <shared/drivers/config/board.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

/* N0100 userland DFU can't write:
 * - the SRAM allocated for the kernel
 * - the Userland stack where the DFU driver is actually living (it cannot
 *   override itself) */

constexpr static uint32_t WritableSRAMStartAddress = UserlandSRAMAddress;
constexpr static uint32_t WritableSRAMEndAddress = UserlandSRAMAddress + UserlandSRAMLength - UserlandStackLength;

}
}
}
}

#endif
