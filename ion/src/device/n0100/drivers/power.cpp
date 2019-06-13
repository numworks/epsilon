#include <drivers/power.h>
#include <drivers/board.h>
#include <drivers/wakeup.h>

namespace Ion {
namespace Power {

void standby() {
  Device::Power::waitUntilOnOffKeyReleased();
  Device::Power::stopConfiguration();
  Device::Board::shutdownPeripherals();
  Device::WakeUp::onOnOffKeyDown();
  Device::Power::internal_flash_standby();
}

}
}
