#include <drivers/power.h>
#include <drivers/svcall.h>

namespace Ion {
namespace Power {

/* We isolate the standby code that needs to be executed from the internal
 * flash (because the external flash is then shut down). We forbid inlining to
 * avoid inlining these instructions in the external flash. */

void standby() {
  Device::Power::waitUntilOnOffKeyReleased();
  svc(SVC_POWER_STANDBY);
}

}
}
