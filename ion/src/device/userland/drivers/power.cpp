#include <drivers/svcall.h>
#include <ion/power.h>

namespace Ion {
namespace Power {

void SVC_ATTRIBUTES selectStandbyMode(bool standbyMode) {
  SVC_RETURNING_VOID(SVC_POWER_SELECT_STANDBY_MODE)
}

void SVC_ATTRIBUTES suspend(bool checkIfOnOffKeyReleased) {
  SVC_RETURNING_VOID(SVC_POWER_SUSPEND)
}

void SVC_ATTRIBUTES standby() {
  SVC_RETURNING_VOID(SVC_POWER_STANDBY)
}

}
}
