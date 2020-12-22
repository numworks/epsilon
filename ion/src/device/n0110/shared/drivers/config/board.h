#ifndef ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_BOARD_H

#include <ion/src/device/shared/drivers/config/board.h>
#include <drivers/config/internal_flash.h>
#include <drivers/config/external_flash.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

constexpr static uint32_t ITCMInterface = 0x00200000;
constexpr static uint32_t AXIMInterface = 0x08000000;
constexpr static uint32_t StandardBootloaderStartAddress = ITCMInterface;
constexpr static uint32_t RescueBootloaderStartAddress = ITCMInterface + BootloaderLength;
constexpr static uint32_t KernelFirstBlockStartAddress = ITCMInterface + 2*BootloaderLength;
constexpr static uint32_t KernelSecondBlockStartAddress = ExternalFlash::Config::StartAddress + ExternalFlash::Config::FirstSectorSize;

}
}
}
}

#endif
