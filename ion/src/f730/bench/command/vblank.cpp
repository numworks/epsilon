#include "command.h"
#include <ion.h>
#include <ion/src/device/display.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void VBlank(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }

  for (int i=0; i<6; i++) {
    Ion::Display::waitForVBlank();
  }

  reply(sOK);
}

}
}
}
}
