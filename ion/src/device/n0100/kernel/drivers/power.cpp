#include <drivers/power.h>
#include <drivers/reset.h>
#include <drivers/wakeup.h>
#include <kernel/drivers/board.h>
#include <ion/src/shared/keyboard_queue.h>

namespace Ion {
namespace Device {
namespace Power {

void standby() {
  /* N0100 cannot use standby mode because the power key is not correctly wired
   * to a wake up pin in order to trigger a wake up event. If the device is in
   * standby, we won't be able to wake it up. Instead, we use the stop mode (as
   * it is the low power mode consuming less after standby mode). */
  selectStandbyMode(false);
  suspend(true);
}

void configWakeUp() {
  WakeUp::onOnOffKeyDown();
  WakeUp::onUSBPlugging();
}

void bootloaderSuspend() {
  /* To enter sleep, we need to issue a WFE instruction, which waits for the
   * event flag to be set and then clears it. However, the event flag might
   * already be on. So the safest way to make sure we actually wait for a new
   * event is to force the event flag to on (SEV instruction), use a first WFE
   * to clear it, and then a second WFE to wait for a _new_ event. */
  asm("sev");
  asm("wfe");
  asm("nop");
  asm("wfe");
}

}
}
}

