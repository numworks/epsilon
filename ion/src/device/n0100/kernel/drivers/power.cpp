#include <drivers/power.h>
#include <drivers/wakeup.h>
#include <kernel/drivers/board.h>

namespace Ion {
namespace Device {
namespace Power {

void standby() {
  Power::waitUntilOnOffKeyReleased();
  /* N0100 cannot use standby mode because the power key is not correctly wired
   * to a wake up pin in order to trigger a wake up event. If the device is in
   * standby, we won't be able to wake it up. Instead, we use the stop mode (as
   * it is the low power mode consuming less after standby mode). */
  Power::stopConfiguration();
  Board::shutdownPeripherals();
  WakeUp::onOnOffKeyDown();
  Power::shutdownPeripheralsClocks(false);
  bootloaderSuspend();
}

void configWakeUp() {
  WakeUp::onOnOffKeyDown();
  WakeUp::onUSBPlugging();
}

}
}
}

