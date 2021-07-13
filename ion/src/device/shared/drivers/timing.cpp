#include <ion/timing.h>
#include <drivers/board_privileged.h>
#include <drivers/config/timing.h>

namespace Ion {
namespace Timing {

using namespace Ion::Device::Timing;

void usleep(uint32_t us) {
 for (volatile uint32_t i=0; i < Config::LoopsPerMicrosecond*us; i++) {
    __asm volatile("nop");
  }
}

void msleep(uint32_t ms) {
  // We decrease the AHB clock frequency to save power while sleeping.
  Device::Board::setClockLowFrequency();
  for (volatile uint32_t i=0; i < Config::LoopsPerMillisecondLowFrequency*ms; i++) {
    __asm volatile("nop");
  }
  Device::Board::setClockStandardFrequency();
}

}
}
