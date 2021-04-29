#ifndef ION_DEVICE_SHARED_INTERNAL_FLASH_OTP_H
#define ION_DEVICE_SHARED_INTERNAL_FLASH_OTP_H

#include <shared/drivers/internal_flash.h>

namespace Ion {
namespace Device {
namespace InternalFlash {

uint32_t readOTPAtIndex(int block, int index);
void writeOTPAtIndex(int block, int index, uint32_t value);
bool isLockedOTPAtBlockIndex(int block);
void lockOTPAtBlockIndex(int block);

}
}
}

#endif
