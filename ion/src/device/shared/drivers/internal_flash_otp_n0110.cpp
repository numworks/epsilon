#include <config/internal_flash.h>
#include <drivers/internal_flash.h>

namespace Ion {
namespace Device {
namespace InternalFlash {

bool validLocation(int block, int index) {
  return block >= 0 && block < Config::NumberOfOTPBlocks && index >= 0 && index < static_cast<int>(Config::OTPBlockSize/sizeof(uint32_t));
}

uint32_t readOTPAtIndex(int block, int index) {
  if (!validLocation(block, index)) {
    return 0;
  }
  return *reinterpret_cast<uint32_t *>(Config::OTPAddress(block, index));
}

void writeOTPAtIndex(int block, int index, uint32_t value) {
  if (!validLocation(block, index)) {
    return;
  }
  uint8_t * destination = reinterpret_cast<uint8_t *>(Config::OTPAddress(block, index));
  InternalFlash::WriteMemory(destination, reinterpret_cast<uint8_t *>(&value), sizeof(value));
}

bool isLockedOTPAtBlockIndex(int block) {
  return *reinterpret_cast<const uint8_t *>(InternalFlash::Config::OTPLockAddress(block)) == 0;
}

void lockOTPAtBlockIndex(int block) {
  uint8_t * destination = reinterpret_cast<uint8_t *>(InternalFlash::Config::OTPLockAddress(block));
  uint8_t zero = 0;
  InternalFlash::WriteMemory(destination, &zero, sizeof(zero));
}

}
}
}
