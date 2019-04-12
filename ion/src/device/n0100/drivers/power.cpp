#include <drivers/power.h>
#include <drivers/board.h>
#include <drivers/reset.h>
#include <drivers/wakeup.h>

namespace Ion {
namespace Power {

void standby() {
  Device::Power::waitUntilOnOffKeyReleased();
  Device::Power::stopConfiguration();
  Device::Board::shutdownPeripherals();
  Device::WakeUp::onOnOffKeyDown();
  Device::Board::shutdownClocks();
  Device::Power::enterLowPowerMode();
  Device::Reset::core();
}

}
}
