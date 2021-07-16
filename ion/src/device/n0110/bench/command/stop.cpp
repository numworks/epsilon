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
  Power::stopConfiguration();
  Board::shutdownPeripherals();
  WakeUp::onUSBPlugging();
  Power::bootloaderSuspend();
  Board::initPeripherals();
}

}
}
}
}
