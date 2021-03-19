#include "timing.h"
#include <drivers/config/clocks.h>

namespace Ion {
namespace Device {
namespace Timing {

using namespace Regs;

volatile uint64_t MillisElapsed = 0;

void initInterruptions() {
  CORTEX.SYST_CSR()->setTICKINT(true);
  CORTEX.SYST_CSR()->setENABLE(true);
}

void shutdownInterruptions() {
  CORTEX.SYST_CSR()->setENABLE(false);
  CORTEX.SYST_CSR()->setTICKINT(false);
}

void init() {
  setSysTickFrequency(Clocks::Config::HCLKFrequency);
  CORTEX.SYST_CSR()->setCLKSOURCE(CORTEX::SYST_CSR::CLKSOURCE::AHB_DIV8);
  initInterruptions();
}

void shutdown() {
  shutdownInterruptions();
}

void setSysTickFrequency(int frequencyInMHz) {
  /* Systick clock source is the CPU clock HCLK divided by 8. To get 1 ms
   * systick overflow, we need to set it to :
   * (HCLK (MHz) * 1 000 000 / 8 )/ 1000 (ms/s) = HCLK (MHz) * 1000 / 8. */
  int SysTickPerMillisecond = frequencyInMHz * 1000 / 8;
  Ion::Device::Regs::CORTEX.SYST_RVR()->setRELOAD(SysTickPerMillisecond - 1); // Remove 1 because the counter resets *after* counting to 0
  Ion::Device::Regs::CORTEX.SYST_CVR()->setCURRENT(0); // setCURRENT will set the value to 0, no matter the argument, as per the CORTEX user guide
}

}
}
}

namespace Ion {
namespace Timing {

uint64_t millis() {
  return Device::Timing::MillisElapsed;
}

}
}
