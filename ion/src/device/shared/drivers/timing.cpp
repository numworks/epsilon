#include "timing.h"
#include <ion/timing.h>
#include <drivers/config/timing.h>

namespace Ion {
namespace Timing {

using namespace Ion::Device::Timing;

/* TODO: The delay methods 'msleep' and 'usleep' are currently dependent on the
 * optimizations chosen by the compiler. To prevent that and to gain in
 * precision, we could use the controller cycle counter (Systick). */

void msleep(uint32_t ms) {
  for (volatile uint32_t i=0; i<Config::LoopsPerMillisecond*ms; i++) {
      __asm volatile("nop");
  }
}
void usleep(uint32_t us) {
  for (volatile uint32_t i=0; i<Config::LoopsPerMicrosecond*us; i++) {
    __asm volatile("nop");
  }
}

uint64_t millis() {
  return MillisElapsed;
}

}
}

namespace Ion {
namespace Device {
namespace Timing {

volatile uint64_t MillisElapsed = 0;

void init() {
  CM4.SYST_RVR()->setRELOAD(Config::SysTickPerMillisecond - 1); // Remove 1 because the counter resets *after* counting to 0
  CM4.SYST_CVR()->setCURRENT(0);
  CM4.SYST_CSR()->setCLKSOURCE(CM4::SYST_CSR::CLKSOURCE::AHB_DIV8);
  CM4.SYST_CSR()->setTICKINT(true);
  CM4.SYST_CSR()->setENABLE(true);
}

void shutdown() {
  CM4.SYST_CSR()->setENABLE(false);
  CM4.SYST_CSR()->setTICKINT(false);
}

}
}
}
