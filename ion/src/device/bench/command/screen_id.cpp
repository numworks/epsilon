#include "command.h"
#include <ion.h>
#include <ion/src/device/shared/drivers/display.h>
#include <poincare/print_int.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void ScreenID(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  uint32_t screenID = Ion::Device::Display::panelIdentifier();

  constexpr int bufferSize = 20;
  char buffer[bufferSize];
  for (int i = 0; i < bufferSize; i++) {
    buffer[i] = 0;
  }
  Poincare::PrintInt::PadIntInBuffer(screenID, buffer, bufferSize - 1);
  reply(buffer);
}

}
}
}
}
