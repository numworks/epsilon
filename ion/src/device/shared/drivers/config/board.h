#ifndef ION_DEVICE_SHARED_DRIVERS_CONFIG_BOARD_H
#define ION_DEVICE_SHARED_DRIVERS_CONFIG_BOARD_H

#include <stdint.h>
#include <stddef.h>
#include <drivers/config/external_flash.h>
#include <drivers/hash.h>

namespace Ion {
namespace Device {
namespace Board {
namespace Config {

constexpr static uint32_t STBootloaderAddress = 0x00100000;
constexpr static uint32_t BootloaderLength = 0x4000;
constexpr static uint32_t KernelInternalLength = 0x8000;
constexpr static size_t SizeSize = sizeof(uint32_t);
constexpr static size_t SignatureSize = Hash::Sha256DigestBytes;


}
}
}
}

#endif
