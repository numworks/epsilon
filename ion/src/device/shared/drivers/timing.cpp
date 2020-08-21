#include "board.h"
#include "timing.h"
#include <ion/timing.h>
#include <drivers/config/timing.h>

namespace Ion {

namespace Timing {

using namespace Device::Timing;

// Warning: this is not a real definition
volatile uint64_t MillisElapsed = 0;

uint64_t millis() {
  return MillisElapsed;
}

void usleep(uint32_t us) {
 for (volatile uint32_t i=0; i<Config::LoopsPerMicrosecond*us; i++) {
    __asm volatile("nop");
  }
}

/* TODO: The delay methods 'msleep' and 'usleep' are currently dependent on the
 * optimizations chosen by the compiler. To prevent that and to gain in
 * precision, we could use the controller cycle counter (Systick). */

void msleep(uint32_t ms) {
  // We decrease the AHB clock frequency to save power while sleeping.
  Device::Board::setClockLowFrequency();
  for (volatile uint32_t i=0; i<Config::LoopsPerMillisecondLowFrequency*ms; i++) {
    __asm volatile("nop");
  }
  Device::Board::setClockStandardFrequency();
}

}

}
