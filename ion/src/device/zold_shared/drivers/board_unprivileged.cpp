#include "board.h"
#include "svcall.h"

namespace Ion {
namespace Device {
namespace Board {

void setClockLowFrequency() {
  svc(SVC_CLOCK_LOW_FREQUENCY);
}

void setClockStandardFrequency() {
  svc(SVC_CLOCK_STANDARD_FREQUENCY);
}


}
}
}
