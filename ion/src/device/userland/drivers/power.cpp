#include <ion/power.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Power {

void SVC_ATTRIBUTES selectStandbyMode(bool standbyMode) {
  SVC(SVC_POWER_SELECT_STANDBY_MODE);
}

void SVC_ATTRIBUTES suspend(bool checkIfOnOffKeyReleased) {
  SVC(SVC_POWER_SUSPEND);
}

void SVC_ATTRIBUTES standby() {
  SVC(SVC_POWER_STANDBY);
}

}
}
