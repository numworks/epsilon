#include "command.h"
#include <drivers/power.h>
#include <drivers/board.h>
#include <drivers/wakeup.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Standby(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  reply(sOK);
  Device::Power::standbyConfiguration();
  Device::Board::shutdown();
  Device::Power::enterLowPowerMode();
  assert(false);
}

}
}
}
}
