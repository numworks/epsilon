#ifndef ION_DEVICE_KERNEL_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_KERNEL_DRIVERS_CONFIG_BOARD_H

#include <stdint.h>
#include <drivers/board.h>
#include <drivers/config/board.h>
#include <drivers/config/external_flash.h>
#include <drivers/config/persisting_bytes.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

/* External flash memory layout:
 * | EXAM MODE BUFFER - SIZE - PARTIAL KERNEL - SIGNATURE | SIZE - USERLAND - SIGNATURE |
 */

constexpr static uint32_t KernelExternalLength = 64*1024 - PersistingBytes::Config::BufferSize - SizeSize - SignatureSize; // TODO Emilie: Compute from config_flash.ld??
constexpr static uint32_t UserlandAddress = ExternalFlash::Config::StartAddress + PersistingBytes::Config::BufferSize + 2*SizeSize + KernelExternalLength + SignatureSize;

}
}
}
}

#endif

