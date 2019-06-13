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
  Device::Power::internal_flash_standby();
  assert(false);
}

}
}
}
}
