#include "power_suspend.h"
#include <drivers/board.h>
#include <drivers/external_flash.h>
#include <shared/drivers/power.h>
#include <shared/drivers/power_configuration.h>

namespace Ion {
namespace Device {
namespace Power {

void standby() {
  standbyConfiguration();
  suspend();
}

void suspend() {
  ExternalFlash::shutdown();
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
  ExternalFlash::init();
}

}
}
}
