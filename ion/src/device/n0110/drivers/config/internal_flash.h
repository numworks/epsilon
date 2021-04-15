#ifndef ION_DEVICE_N0110_CONFIG_INTERNAL_FLASH_H
#define ION_DEVICE_N0110_CONFIG_INTERNAL_FLASH_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace InternalFlash {
namespace Config {

constexpr static uint32_t StartAddress = 0x08000000;
constexpr static uint32_t EndAddress = 0x08010000;
constexpr static int NumberOfSectors = 4;
constexpr static uint32_t SectorAddresses[NumberOfSectors+1] = {
  0x08000000, 0x08004000, 0x08008000, 0x0800C000,
  0x08010000
};

constexpr static uint32_t OTPStartAddress = 0x1FF07800;
constexpr static uint32_t OTPLocksAddress = 0x1FF07A00;
constexpr static int NumberOfOTPBlocks = 16;
constexpr static uint32_t OTPBlockSize = 0x20;
constexpr uint32_t OTPAddress(int block) { return OTPStartAddress + block * OTPBlockSize; };
constexpr uint32_t OTPLockAddress(int block) { return OTPLocksAddress + block; }

}
}
}
}

#endif
