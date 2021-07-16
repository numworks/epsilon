#include <drivers/power.h>
#include <drivers/board.h>
#include <drivers/keyboard.h>
#include <drivers/wakeup.h>
#include <shared/drivers/power.h>

namespace Ion {
namespace Device {
namespace Power {

/* We isolate the standby code that needs to be executed from the internal
 * flash (because the external flash is then shut down). We forbid inlining to
 * avoid inlining these instructions in the external flash. */

void configWakeUp() {
  WakeUp::onOnOffKeyDown();
  WakeUp::onUSBPlugging();
  WakeUp::onChargingEvent();
}

}
}
}
