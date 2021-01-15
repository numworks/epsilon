#include "timing.h"
#include <drivers/board.h>
#include <drivers/config/timing.h>

namespace Ion {
namespace Device {
namespace Timing {

void usleep(uint32_t us) {
 for (volatile uint32_t i=0; i<Config::LoopsPerMicrosecond*us; i++) {
    __asm volatile("nop");
  }
}

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
}
