#include <ion/power.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Power {

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
