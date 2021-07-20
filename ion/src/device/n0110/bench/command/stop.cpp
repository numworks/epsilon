#include "command.h"
#include <drivers/board.h>
#include <drivers/power.h>
#include <drivers/power_suspend.h>
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
  Power::stopConfiguration();
  Board::shutdownPeripherals();
  WakeUp::onUSBPlugging();
  Power::suspend();
  Board::initPeripherals();
}

}
}
}
}
