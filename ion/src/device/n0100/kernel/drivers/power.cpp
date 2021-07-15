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
  Board::shutdownPeripheralsClocks(false);
  bootloaderSuspend();
}

void configWakeUp() {
  WakeUp::onOnOffKeyDown();
  WakeUp::onUSBPlugging();
}

void bootloaderSuspend() {
  Board::shutdownPeripheralsClocks();

  /* To enter sleep, we need to issue a WFE instruction, which waits for the
   * event flag to be set and then clears it. However, the event flag might
   * already be on. So the safest way to make sure we actually wait for a new
   * event is to force the event flag to on (SEV instruction), use a first WFE
   * to clear it, and then a second WFE to wait for a _new_ event. */
  asm("sev");
  asm("wfe");
  asm("nop");
  asm("wfe");

  Board::initPeripheralsClocks();
}

}
}
}

