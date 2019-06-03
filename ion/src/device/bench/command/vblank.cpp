#include "command.h"
#include <ion.h>
#include <ion/src/device/shared/drivers/display.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void VBlank(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  reply(Shared::POSTAndHardwareTests::VBlankOK() ? sOK : sKO);
}

}
}
}
}
