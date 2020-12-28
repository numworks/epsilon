#ifndef ION_DEVICE_KERNEL_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_KERNEL_DRIVERS_CONFIG_BOARD_H

#include <stdint.h>
#include <drivers/board.h>
#include <drivers/config/external_flash.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

constexpr static uint32_t UserlandAddress = ExternalFlash::Config::StartAddress + 0x4000; // TODO EMILIE: compute the right offset and factorize with linker script

}
}
}
}

#endif

