#include "command.h"
#include <drivers/power.h>
#include <drivers/board.h>
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
  Device::Power::sleepConfiguration();
  Device::Board::shutdownPeripherals();
  Device::WakeUp::onUSBPlugging();
  Device::ExternalFlash::shutdown();
  Device::Board::shutdownClocks();
  Device::Power::enterLowPowerMode();
  Device::Board::setStandardFrequency(Device::Board::Frequency::High);
  Device::Board::initClocks();
  Device::Board::initPeripherals(false);
}

}
}
}
}
