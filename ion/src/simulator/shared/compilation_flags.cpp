#include <ion.h>
#include <omg/print.h>

#if !defined(DEBUG) || !defined(ASSERTIONS) || !defined(EXTERNAL_APPS_API_LEVEL)
#error This file expects ASSERTIONS & EXTERNAL_APPS_API_LEVEL to be defined
#endif

namespace Ion {

/*
 * We use an uint32_t as a bit array representing the compilation flags.
 *
 * bit 0: DEBUG
 * bit 1: ASSERTIONS
 * bit 2: allow third-party
 * bit 3: _
 * bits 4 to 7: least significant bits of EXTERNAL_APPS_API_LEVEL
 * bits 8 to 31: _
 *
 * */

uint16_t userlandCompilationFlags() {
  assert(EXTERNAL_APPS_API_LEVEL < 0xFF); // Should the EXTERNAL_APPS_API_LEVEL exceed 0xFF, we'll find another way to represent it
  uint8_t externalAppsAPILevel = 0xFF & EXTERNAL_APPS_API_LEVEL;

  return DEBUG |
         (ASSERTIONS << 1) |
         (ExternalApps::allowThirdParty() << 2) |
         (externalAppsAPILevel << 4);
}

const char * compilationFlags() {
  static char compilationFlagsBuffer[OMG::Print::MaxLengthOfUInt32(OMG::Base::Hexadecimal) + 1] = {0};
  uint32_t flags = userlandCompilationFlags();
  size_t length = OMG::Print::UInt32(OMG::Base::Hexadecimal, flags, OMG::Print::LeadingZeros::Keep, compilationFlagsBuffer, sizeof(compilationFlagsBuffer));
  compilationFlagsBuffer[length] = 0;
  return compilationFlagsBuffer;
}

const char * runningBootloader() {
  return "NA";
}

}
