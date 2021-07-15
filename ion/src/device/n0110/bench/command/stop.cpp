#include "command.h"
#include <drivers/board.h>
#include <drivers/power.h>
#include <drivers/wakeup.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Stop(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  reply(sOK);
  Device::Power::stopConfiguration();
  Device::Board::shutdownPeripherals();
  Device::WakeUp::onUSBPlugging();
  Device::Power::internalFlashSuspend();
  Device::Board::initPeripherals(false);
}

}
}
}
}
