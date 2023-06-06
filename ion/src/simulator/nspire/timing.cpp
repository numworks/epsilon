#include <ion/timing.h>
#include <chrono>
#include "k_csdk.h"

namespace Ion {
namespace Timing {

uint64_t millis() {
  return ::millis();
}

void usleep(uint32_t us) {
  os_wait_1ms(us/1000); // sleep_us(us);
}

void msleep(uint32_t ms) {
  os_wait_1ms(ms); // sleep_us(ms * 1000);
}

}
}
