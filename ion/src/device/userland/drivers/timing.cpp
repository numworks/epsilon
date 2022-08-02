#include <ion/timing.h>
#include <drivers/svcall.h>

namespace Ion {
namespace Timing {

void SVC_ATTRIBUTES usleep(uint32_t us) {
  SVC_RETURNING_VOID(SVC_TIMING_USLEEP)
}

void SVC_ATTRIBUTES msleep(uint32_t ms) {
  SVC_RETURNING_VOID(SVC_TIMING_MSLEEP)
}

uint64_t SVC_ATTRIBUTES millis() {
  SVC_RETURNING_R0R1(SVC_TIMING_MILLIS, uint64_t)
}

}
}
