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

  constexpr int bufferSize = 10 + 1; // screenID is a uint32_t so 10 digits max
  char buffer[bufferSize];
  for (int i = 0; i < bufferSize; i++) {
    buffer[i] = 0;
  }
  Poincare::PrintInt::Left(screenID, buffer, bufferSize - 1);
  reply(buffer);
}

}
}
}
}
