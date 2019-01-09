#include "timing.h"

namespace Ion {
namespace Timing {

/* TODO: The delay methods 'msleep' and 'usleep' are currently dependent on the
 * optimizations chosen by the compiler. To prevent that and to gain in
 * precision, we could use the controller cycle counter (Systick). */

void msleep(uint32_t ms) {
  for (volatile uint32_t i=0; i<8852*ms; i++) {
      __asm volatile("nop");
  }
}
void usleep(uint32_t us) {
  for (volatile uint32_t i=0; i<9*us; i++) {
    __asm volatile("nop");
  }
}

uint64_t millis() {
  return Ion::Timing::Device::MillisElapsed;
}

}
}

namespace Ion {
namespace Timing {
namespace Device {

volatile uint64_t MillisElapsed = 0;

}
}
}
