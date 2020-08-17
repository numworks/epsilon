#include <drivers/config/timing.h>
#include <drivers/config/clocks.h>
#include <drivers/board.h>

namespace Ion {
namespace Timing {

using namespace Device::Timing;

/* TODO: The delay methods 'msleep' and 'usleep' are currently dependent on the
 * optimizations chosen by the compiler. To prevent that and to gain in
 * precision, we could use the controller cycle counter (Systick). */

void msleep(uint32_t ms) {
  // We decrease the AHB clock frequency to save power while sleeping.
  Device::Board::setClockFrequency(Device::Board::Frequency::Low);
  for (volatile uint32_t i=0; i<Config::LoopsPerMillisecondLowFrequency*ms; i++) {
    __asm volatile("nop");
  }
  Device::Board::setClockFrequency(Device::Board::standardFrequency());
}

}
}
