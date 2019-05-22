#include "timing.h"
#include "board.h"
#include <ion/timing.h>
#include <drivers/config/timing.h>

namespace Ion {

namespace Timing {

using namespace Device::Timing;

/* TODO: The delay methods 'msleep' and 'usleep' are currently dependent on the
 * optimizations chosen by the compiler. To prevent that and to gain in
 * precision, we could use the controller cycle counter (Systick). */

void msleep(uint32_t ms) {
  // We decrease the AHB clock frequency to save power while sleeping.
  Device::Board::setClockFrequency(Device::Board::Frequency::Low);
  for (volatile uint32_t i=0; i<Config::LoopsPerMillisecond*ms; i++) {
      __asm volatile("nop");
  }
  Device::Board::setClockFrequency(Device::Board::standardFrequency());
}
void usleep(uint32_t us) {
  for (volatile uint32_t i=0; i<Config::LoopsPerMicrosecond*us; i++) {
    __asm volatile("nop");
  }
}

volatile uint64_t millis() {
  return MillisElapsed;
}

}
}

namespace Ion {
namespace Device {
namespace Timing {

using namespace Regs;

volatile uint64_t MillisElapsed = 0;

void init() {
  CORTEX.SYST_RVR()->setRELOAD(Config::SysTickPerMillisecond - 1); // Remove 1 because the counter resets *after* counting to 0
  CORTEX.SYST_CVR()->setCURRENT(0);
  CORTEX.SYST_CSR()->setCLKSOURCE(CORTEX::SYST_CSR::CLKSOURCE::AHB_DIV8);
  CORTEX.SYST_CSR()->setTICKINT(true);
  CORTEX.SYST_CSR()->setENABLE(true);
}

void shutdown() {
  CORTEX.SYST_CSR()->setENABLE(false);
  CORTEX.SYST_CSR()->setTICKINT(false);
}

}
}
}
