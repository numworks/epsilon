#include <drivers/power.h>
#include <drivers/board.h>
#include <drivers/wakeup.h>

namespace Ion {
namespace Power {

void standby() {
  Device::Power::waitUntilOnOffKeyReleased();
  /* N0100 cannot use standby mode because the power key is not correctly wired
   * to a wake up pin in order to trigger a wake up event. If the device is in
   * standby, we won't be able to wake it up. Instead, we use the stop mode (as
   * it is the low power mode consuming less after standby mode). */
  Device::Power::stopConfiguration();
  Device::Board::shutdownPeripherals();
  Device::WakeUp::onOnOffKeyDown();
  Device::Power::internalFlashStandby();
}

}
}

namespace Ion {
namespace Device {
namespace Power {

void configWakeUp() {
  Device::WakeUp::onOnOffKeyDown();
  Device::WakeUp::onUSBPlugging();
}

}
}
}
