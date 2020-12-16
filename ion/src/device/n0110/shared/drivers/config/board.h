#ifndef ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_BOARD_H

#include <shared/drivers/config/board.h>
#include <drivers/config/internal_flash.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

constexpr static uint32_t ITCMInterface = 0x00200000;
constexpr static uint32_t AXIMInterface = 0x08000000;
constexpr static uint32_t VirtualInternalStartAddress = ITCMInterface;
constexpr static uint32_t VirtualInternalEndAddress = VirtualInternalStartAddress + InternalFlash::Config::EndAddress - InternalFlash::Config::StartAddress;

}
}
}
}

#endif
