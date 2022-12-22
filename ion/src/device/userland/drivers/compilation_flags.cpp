#include <drivers/svcall.h>
#include <shared/drivers/board_shared.h>
#include <ion.h>
#include <omg/print.h>

#if !defined(ASSERTIONS) || !defined(EXTERNAL_APPS_API_LEVEL)
#error This file expects ASSERTIONS & EXTERNAL_APPS_API_LEVEL to be defined
#endif

namespace Ion {

/*
 * We use an uint32_t as a bit array representing the compilation flags. The most significant 16 bits concern the kernel compilation and the least significant 16 bits concern the userland.
 *
 * bit 0: userland DEBUG
 * bit 1: userland ASSERTIONS
 * bit 2: allow third-party
 * bit 3: running slot A
 * bits 4 to 7: least significant bits of EXTERNAL_APPS_API_LEVEL
 * bits 8 to 11: least significant bits of SECURITY_LEVEL
 * bits 12 to 15: _
 * bit 16: kernel NDEBUG
 * bit 17: kernel ASSERTIONS
 * bit 18: kernel IN_FACTORY
 * bit 19: kernel EMBED_EXTRA_DATA
 * bits 20 to 31: _
 *
 * */

uint16_t SVC_ATTRIBUTES kernelCompilationFlags() {
  SVC_RETURNING_R0(SVC_COMPILATION_FLAGS, uint16_t)
}

uint32_t SVC_ATTRIBUTES bootloaderCRC32() {
  SVC_RETURNING_R0(SVC_BOOTLOADER_CRC32, uint32_t)
}

uint16_t userlandCompilationFlags() {
#ifdef NDEBUG
  bool debug = false;
#else
  bool debug = true;
#endif
  assert(EXTERNAL_APPS_API_LEVEL < 0xFF); // Should the EXTERNAL_APPS_API_LEVEL exceed 0xFF, we'll find another way to represent it
  uint8_t externalAppsAPILevel = 0xFF & EXTERNAL_APPS_API_LEVEL;
  assert(Device::Board::securityLevel() < 0xFF); // Should the SECURITY_LEVEL exceed 0xFF, , we'll find another way to represent it
  uint8_t securityLevel = 0xFF & Device::Board::securityLevel();

  return debug |
         (ASSERTIONS << 1) |
         (ExternalApps::allowThirdParty() << 2) |
         (Device::Board::isRunningSlotA() << 3) |
         (externalAppsAPILevel << 4) |
         (securityLevel << 8);
}

const char * compilationFlags() {
  static char compilationFlagsBuffer[OMG::Print::MaxLengthOfUInt32(OMG::Base::Hexadecimal) + 1] = {0};
  uint32_t flags = userlandCompilationFlags() | (kernelCompilationFlags() << 16);
  size_t length = OMG::Print::UInt32(OMG::Base::Hexadecimal, flags, true, compilationFlagsBuffer, sizeof(compilationFlagsBuffer));
  compilationFlagsBuffer[length] = 0;
  return compilationFlagsBuffer;
}

const char * runningBootloader() {
  static char crcBuffer[OMG::Print::MaxLengthOfUInt32(OMG::Base::Hexadecimal) + 1] = {0};
  size_t length = OMG::Print::UInt32(OMG::Base::Hexadecimal, bootloaderCRC32(), true, crcBuffer, sizeof(crcBuffer));
  crcBuffer[length] = 0;
  return crcBuffer;
}

}
