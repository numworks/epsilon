#include "board.h"
#include "timing.h"
#include <ion/timing.h>
#include <drivers/config/timing.h>

namespace Ion {

namespace Timing {

using namespace Device::Timing;

uint64_t millis() {
  int argc = 0;
  svcall(SVC_CLOCK_LOW_FREQUENCY, );
  
  return MillisElapsed;
}

}

}
