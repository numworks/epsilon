#ifndef ION_DEVICE_SHARED_DRIVERS_CONFIG_INTERNAL_FLASH_H
#define ION_DEVICE_SHARED_DRIVERS_CONFIG_INTERNAL_FLASH_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace InternalFlash {
namespace Config {

constexpr static uint32_t OTPStartAddress = 0x1FF07800;
constexpr static uint32_t OTPLocksAddress = 0x1FF07A00;
constexpr static int NumberOfOTPBlocks = 16;
constexpr static uint32_t OTPBlockSize = 0x20;
constexpr uint32_t OTPAddress(int block, int index) { return OTPStartAddress + block * OTPBlockSize + index * sizeof(uint32_t); };

}
}
}
}

#endif
