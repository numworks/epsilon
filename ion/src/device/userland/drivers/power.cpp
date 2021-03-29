#include <ion/power.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Power {

void SVC_ATTRIBUTES selectStandbyModeSVC(bool * standbyMode) {
  SVC(SVC_POWER_SELECT_STANDBY_MODE);
}

void selectStandbyMode(bool standbyMode) {
  selectStandbyModeSVC(&standbyMode);
}

void SVC_ATTRIBUTES suspendSVC(bool * checkIfOnOffKeyReleased) {
  SVC(SVC_POWER_SUSPEND);
}

void suspend(bool checkIfOnOffKeyReleased) {
  suspendSVC(&checkIfOnOffKeyReleased);
}

void standby() {
  SVC(SVC_POWER_STANDBY);
}

}
}
