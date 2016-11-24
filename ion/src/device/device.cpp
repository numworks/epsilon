#include "device.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
}
#include <ion.h>
#include "led.h"
#include "display.h"
#include "keyboard.h"
#include "battery.h"
#include "sd_card.h"

#define USE_SD_CARD 0

// Public Ion methods

void Ion::msleep(long ms) {
  for (volatile long i=0; i<1040*ms; i++) {
      __asm volatile("nop");
  }
}

void Ion::reset() {
  CM4.AIRCR()->requestReset();
}

// Private Ion::Device methods

void Ion::Device::init() {
  initFPU();
  initClocks();
  LED::Device::init();
  Display::Device::init();
  Keyboard::Device::init();
  Battery::Device::init();

  Display::setBacklightIntensity(0xFF);
#if USE_SD_CARD
  SDCard::Device::init();
#endif
}

void Ion::Device::initClocks() {
#define USE_96MHZ_SYSTEM_CLOCK 0
#if USE_96MHZ_SYSTEM_CLOCK
  /* System clock
   * Configure the CPU at 96 MHz, APB2 and USB at 48 MHz. */

  /* After reset the Flash runs as fast as the CPU. When we clock the CPU faster
   * the flash memory cannot follow and therefore flash memory accesses need to
   * wait a little bit.
   * The spec tells us that at 2.8V and over 90MHz the flash expects 3 WS. */
  FLASH.ACR()->setLATENCY(3);

  /* We're using the high-speed internal oscillator as a clock source. It runs
   * at a fixed 16 MHz frequency, but by piping it through the PLL we can derive
   * faster oscillations. Combining default values and a PLLQ of 4 can provide
   * us with a 96 MHz frequency for SYSCLK. */
  RCC.PLLCFGR()->setPLLQ(4);
  RCC.PLLCFGR()->setPLLSRC(RCC::PLLCFGR::PLLSRC::HSI);
  // 96 MHz is too fast for APB1. Divide it by two to reach 48 MHz
  RCC.CFGR()->setPPRE1(RCC::CFGR::AHBRatio::DivideBy2);

  // Enable the PLL and wait for it to be ready
  RCC.CR()->setPLLON(true);
  while(!RCC.CR()->getPLLRDY()) {
  }

  // Last but not least, use the PLL output as a SYSCLK source
  RCC.CFGR()->setSW(RCC::CFGR::SW::PLL);
  while (RCC.CFGR()->getSWS() != RCC::CFGR::SW::PLL) {
  }
#else
  // The high-speed internal oscillator runs at 16 MHz. By default we're not
  // using the PLL and would feed 96 MHz into the SDIO bus. The easiest way
  // to enable SDIO access is to just use the system clock (16 MHz) for SDIO.
  RCC.DCKCFGR2()->setCKSDIOSEL(1);
#endif

  // Peripheral clocks

  // AHB1 bus
  // Our peripherals are using GPIO A, B, C, D and E.
  // We're not using the CRC nor DMA engines.
  class RCC::AHB1ENR ahb1enr(0); // Reset value
  ahb1enr.setGPIOAEN(true);
  ahb1enr.setGPIOBEN(true);
  ahb1enr.setGPIOCEN(true);
  ahb1enr.setGPIODEN(true);
  ahb1enr.setGPIOEEN(true);
  RCC.AHB1ENR()->set(ahb1enr);

  // APB1 bus
  // We're using TIM3
  RCC.APB1ENR()->setTIM3EN(true);

  RCC.APB2ENR()->setADC1EN(true);
  RCC.APB2ENR()->setSYSCFGEN(true);
#if USE_SD_CARD
  RCC.APB2ENR()->setSDIOEN(true);
#endif
}

void Ion::Device::initFPU() {
  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BABDBFBJ.html
  CM4.CPACR()->setAccess(10, CM4::CPACR::Access::Full);
  CM4.CPACR()->setAccess(11, CM4::CPACR::Access::Full);
  // FIXME: The pipeline should be flushed at this point
}
