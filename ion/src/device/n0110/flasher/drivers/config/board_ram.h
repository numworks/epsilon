#ifndef ION_DEVICE_N0110_FLASHER_DRIVERS_CONFIG_BOARD_RAM_H
#define ION_DEVICE_N0110_FLASHER_DRIVERS_CONFIG_BOARD_RAM_H

#include <shared/drivers/config/board.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

// Flasher DFU can't write where the flasher is mapped.

constexpr static uint32_t FlasherLength = 0x10000; // 64k
constexpr static uint32_t WritableSRAMStartAddress = SRAMAddress;
constexpr static uint32_t WritableSRAMEndAddress = SRAMAddress + SRAMLength - FlasherLength;

}
}
}
}

#endif
