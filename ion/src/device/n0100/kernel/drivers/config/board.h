#ifndef ION_DEVICE_N0100_KERNEL_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_N0100_KERNEL_DRIVERS_CONFIG_BOARD_H

#include <drivers/config/internal_flash.h>
#include <shared/drivers/config/board.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

constexpr static uint32_t UserlandAddress = InternalFlash::Config::StartAddress + BootloaderTotalSize + PersistingBytes::Config::BufferSize + KernelTotalSize + SizeSize;


}
}
}
}

#endif
