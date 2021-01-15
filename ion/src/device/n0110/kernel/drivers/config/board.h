#ifndef ION_DEVICE_N0110_KERNEL_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_N0110_KERNEL_DRIVERS_CONFIG_BOARD_H

#include <shared/drivers/config/board.h>
#include <drivers/config/external_flash.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

constexpr static uint32_t UserlandAddress = ExternalFlash::Config::StartAddress + PersistingBytes::Config::BufferSize + KernelExternalTotalSize + SizeSize;

}
}
}
}

#endif
