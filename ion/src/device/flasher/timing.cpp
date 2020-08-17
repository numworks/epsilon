#include <drivers/config/timing.h>
#include <drivers/config/clocks.h>
#include <drivers/board.h>

namespace Ion {
namespace Timing {

using namespace Device::Timing;

void msleep(uint32_t ms) {
  // We decrease the AHB clock frequency to save power while sleeping.
  for (volatile uint32_t i=0; i< Config::LoopsPerMillisecondHighFrequency*ms; i++) {
    __asm volatile("nop");
  }
}

}
}
