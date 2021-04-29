#include <drivers/board.h>
#include <drivers/config/clocks.h>
#include <regs/regs.h>

// Private Ion::Device methods

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void initSystemClocks() {
  /* System clock
   * Configure the CPU at 96 MHz, APB2 and USB at 48 MHz. */

  /* After reset the Flash runs as fast as the CPU. When we clock the CPU faster
   * the flash memory cannot follow and therefore flash memory accesses need to
   * wait a little bit.
   * The spec tells us that at 2.8V and over 90MHz the flash expects 3 WS. */
  FLASH.ACR()->setLATENCY(3);

  /* Enable prefetching flash instructions */
  /* Fetching instructions increases slightly the power consumption but the
   * increase is negligible compared to the screen consumption. */
  FLASH.ACR()->setPRFTEN(true);

  /* Set flash instruction and data cache */
  FLASH.ACR()->setDCEN(true);
  FLASH.ACR()->setICEN(true);

  /* After reset, the device is using the high-speed internal oscillator (HSI)
   * as a clock source, which runs at a fixed 16 MHz frequency. The HSI is not
   * accurate enough for reliable USB operation, so we need to use the external
   * high-speed oscillator (HSE). */

  // Enable the HSE and wait for it to be ready
  RCC.CR()->setHSEON(true);
  while(!RCC.CR()->getHSERDY()) {
  }

  /* Given the crystal used on our device, the HSE will oscillate at 25 MHz. By
   * piping it through a phase-locked loop (PLL) we can derive other frequencies
   * for use in different parts of the system. Combining the default PLL values
   * with a PLLM of 25 and a PLLQ of 4 yields both a 96 MHz frequency for SYSCLK
   * and the required 48 MHz USB clock. */

  // Configure the PLL ratios and use HSE as a PLL input
  RCC.PLLCFGR()->setPLLM(Clocks::Config::PLL_M);
  RCC.PLLCFGR()->setPLLQ(Clocks::Config::PLL_Q);
  RCC.PLLCFGR()->setPLLSRC(RCC::PLLCFGR::PLLSRC::HSE);
  // 96 MHz is too fast for APB1. Divide it by two to reach 48 MHz
  RCC.CFGR()->setPPRE1(Clocks::Config::APB1PrescalerRegs);

  // Enable the PLL and wait for it to be ready
  RCC.CR()->setPLLON(true);
  while(!RCC.CR()->getPLLRDY()) {
  }

  // Use the PLL output as a SYSCLK source
  RCC.CFGR()->setSW(RCC::CFGR::SW::PLL);
  while (RCC.CFGR()->getSWS() != RCC::CFGR::SW::PLL) {
  }

  // Now that we don't need use it anymore, turn the HSI off
  RCC.CR()->setHSION(false);^
}

/* The following methods regarding PCB version are dummy implementations.
 * Handling the PCB version is only necessary on the N0110. */

PCBVersion pcbVersion() {
  return PCB_LATEST;
}

PCBVersion readPCBVersionInMemory() {
  return PCB_LATEST;
}

void writePCBVersion(PCBVersion) {}

void lockPCBVersion() {}

bool pcbVersionIsLocked() { return true; }

}
}
}
