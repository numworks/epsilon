#ifndef ION_DEVICE_SHARED_INTERNAL_FLASH_OTP_H
#define ION_DEVICE_SHARED_INTERNAL_FLASH_OTP_H

#include <shared/drivers/internal_flash.h>

namespace Ion {
namespace Device {
namespace InternalFlash {

constexpr static uint32_t OTPStartAddress = 0x1FF07800;
constexpr static uint32_t OTPLocksAddress = 0x1FF07A00;
constexpr static int NumberOfOTPBlocks = 16;
constexpr static uint32_t OTPBlockSize = 0x20;
constexpr uint32_t OTPAddress(int block, int index) { return OTPStartAddress + block * OTPBlockSize + index * sizeof(uint32_t); };
constexpr uint32_t OTPLockAddress(int block) { return OTPLocksAddress + block; }

uint32_t readOTPAtIndex(int block, int index);
void writeOTPAtIndex(int block, int index, uint32_t value);
bool isLockedOTPAtBlockIndex(int block);
void lockOTPAtBlockIndex(int block);

}
}
}

#endif
