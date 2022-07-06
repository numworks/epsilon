#include <ion/timing.h>
#include <config/timing.h>

namespace Ion {
namespace Timing {

/* msleep specifically made for DFU driver: the DFU driver runs relocated on
 * sRAM and cannot make any svcall since the internal flash might be being
 * reflashed. */

void msleep(uint32_t ms) {
  for (volatile uint32_t i=0; i < Device::Timing::Config::LoopsPerMicrosecond*1000; i++) {
    __asm volatile("nop");
  }
}

}
}

