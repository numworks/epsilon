#include <ion/timing.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Timing {

void SVC_ATTRIBUTES usleep(uint32_t us) {
  SVC(SVC_TIMING_USLEEP);
}

void SVC_ATTRIBUTES msleep(uint32_t ms) {
  SVC(SVC_TIMING_MSLEEP);
}

uint64_t SVC_ATTRIBUTES millis() {
  SVC(SVC_TIMING_MILLIS);
}

}
}
