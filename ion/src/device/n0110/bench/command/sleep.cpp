#include "command.h"
#include <drivers/board.h>
#include <drivers/power.h>
#include <drivers/wakeup.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Sleep(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  reply(sOK);
  Power::sleepConfiguration();
  Board::shutdownPeripherals();
  WakeUp::onUSBPlugging();
  Power::bootloaderSuspend();
  Board::setStandardFrequency(Device::Board::Frequency::High);
  Board::initPeripherals();
}

}
}
}
}
