#include <shared/drivers/power.h>
#include <kernel/drivers/power.h>
#include <drivers/wakeup.h>
#include <kernel/drivers/board.h>
#include <kernel/drivers/keyboard.h>

namespace Ion {
namespace Device {
namespace Power {

/* We isolate the standby code that needs to be executed from the internal
 * flash (because the external flash is then shut down). We forbid inlining to
 * avoid inlining these instructions in the external flash. */

void standby() {
  Power::waitUntilOnOffKeyReleased();
  Power::standbyConfiguration();
  Board::shutdownPeripherals();
  Board::shutdownPeripheralsClocks(false);
  bootloaderSuspend();
}

void configWakeUp() {
  WakeUp::onOnOffKeyDown();
  WakeUp::onUSBPlugging();
  WakeUp::onChargingEvent();
}

}
}
}
