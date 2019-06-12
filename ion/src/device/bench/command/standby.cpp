#include "command.h"
#include <drivers/board.h>
#include <drivers/external_flash.h>
#include <drivers/power.h>
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
  Device::Board::shutdownPeripherals();
  Device::ExternalFlash::shutdown();
  Device::Board::shutdownClocks();
  Device::Power::enterLowPowerMode();
  assert(false);
}

}
}
}
}
