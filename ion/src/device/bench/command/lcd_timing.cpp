#include "command.h"
#include <apps/shared/post_and_hardware_tests.h>
#include <poincare/print_int.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void LCDTiming(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  /* TODO: Find better place for LCDDataGlyphFailures than
   * apps/shared/post_and_hardware_tests. Problem is it needs Kandinsky so Ion
   * might not be the best place. Maybe move the bench out of Ion? */
  constexpr int bufferSize = 6+10+1; // crc is a uint32_t so 10 digits long.
  char buffer[bufferSize] = {'D', 'E', 'L', 'T', 'A', '='};
  for (int i  = 6; i < bufferSize; i++) {
    buffer[i] = 0;
  }
  Poincare::PrintInt::Left(Shared::POSTAndHardwareTests::LCDTimingGlyphFailures(), buffer+6, bufferSize - 6 - 1);
  reply(buffer);
}

}
}
}
}
