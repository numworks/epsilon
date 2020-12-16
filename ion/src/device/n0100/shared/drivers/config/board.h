#ifndef ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_BOARD_H

#include <shared/drivers/config/board.h>
#include <drivers/config/internal_flash.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

constexpr static uint32_t VirtualInternalStartAddress = InternalFlash::StartAddress;
constexpr static uint32_t VirtualInternalEndAddress = InternalFlash::EndAddress;

}
}
}
}

#endif
