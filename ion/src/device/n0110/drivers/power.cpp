#include <drivers/power.h>
#include <drivers/board.h>
#include <drivers/external_flash.h>
#include <drivers/reset.h>

namespace Ion {
namespace Power {

/* We isolate the standby code that needs to be executed from the internal
 * flash (because the external flash is then shut down). We forbid inlining to
 * avoid inlining these instructions in the external flash. */

void __attribute__((noinline)) internal_flash_standby() {
  Device::ExternalFlash::shutdown();
  Device::Board::shutdownClocks();
  Device::Power::enterLowPowerMode();
  Device::Reset::core();
}

void standby() {
  Device::Power::waitUntilOnOffKeyReleased();
  Device::Power::standbyConfiguration();
  Device::Board::shutdownPeripherals();
  internal_flash_standby();
}

}
}
