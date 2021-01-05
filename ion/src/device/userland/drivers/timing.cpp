#include <ion/timing.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Timing {

void SVC_ATTRIBUTES usleepSVC(uint32_t * us) {
  SVC(SVC_TIMING_USLEEP);
}

void usleep(uint32_t us) {
  usleepSVC(&us);
}

void SVC_ATTRIBUTES msleepSVC(uint32_t * ms) {
  SVC(SVC_TIMING_USLEEP);
}

void msleep(uint32_t ms) {
  msleepSVC(&ms);
}

void SVC_ATTRIBUTES millisSVC(uint64_t * res) {
  SVC(SVC_TIMING_MILLIS);
}

uint64_t millis() {
  uint64_t res;
  millisSVC(&res);
  return res;
}

}
}
