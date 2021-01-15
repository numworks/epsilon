#include <drivers/power.h>
#include "svcall.h"

namespace Ion {

namespace Power {

void suspend(bool checkIfOnOffKeyReleased) {
  if (checkIfOnOffKeyReleased) {
    Device::Power::waitUntilOnOffKeyReleased();
  }
  svc(SVC_POWER_SLEEP_OR_STOP);
}

}

}
