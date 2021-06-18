#include <drivers/board.h>
#include <drivers/console.h>
#include <drivers/config/clocks.h>
#include <regs/regs.h>
#include <ion.h>
#include <ion/src/device/bench/bench.h>

typedef void(*ISR)(void);
extern ISR InitialisationVector[];

// Public Ion methods

const char * Ion::fccId() {
  return "2ALWP-N0100";
}

// Private Ion::Device methods

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void init() {
  initClocks();

  // Ensure right location of interrupt vectors
  // The bootloader leaves its own after flashing
  SYSCFG.MEMRMP()->setMEM_MODE(SYSCFG::MEMRMP::MemMode::MainFlashmemory);
  CORTEX.VTOR()->setVTOR((void*)&InitialisationVector);

  // Put all inputs as Analog Input, No pull-up nor pull-down
  // Except for the SWD port (PB3, PA13, PA14)
  GPIOA.MODER()->set(0xEBFFFFFF);
  GPIOA.PUPDR()->set(0x24000000);
  GPIOB.MODER()->set(0xFFFFFFBF);
  GPIOB.PUPDR()->set(0x00000000);
  for (int g=2; g<5; g++) {
    GPIO(g).MODER()->set(0xFFFFFFFF); // All to "Analog"
    GPIO(g).PUPDR()->set(0x00000000); // All to "None"
  }
}

void initClocks() {
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
  RCC.CR()->setHSION(false);

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
  ahb1enr.setDMA2EN(true);
  RCC.AHB1ENR()->set(ahb1enr);

  // AHB2 bus
  RCC.AHB2ENR()->setOTGFSEN(true);

  // AHB3 bus
  RCC.AHB3ENR()->setFSMCEN(true);

  // APB1 bus
  // We're using TIM3 for the LEDs
  RCC.APB1ENR()->setTIM3EN(true);
  RCC.APB1ENR()->setPWREN(true);
  RCC.APB1ENR()->setRTCAPB(true);

  // APB2 bus
  class RCC::APB2ENR apb2enr(0x00008000); // Reset value
  apb2enr.setADC1EN(true);
  apb2enr.setSYSCFGEN(true);
#if USE_SD_CARD
  apb2enr.setSDIOEN(true);
#endif
  RCC.APB2ENR()->set(apb2enr);

  // AHB1 peripheral clock enable in low-power mode register
  class RCC::AHB1LPENR ahb1lpenr(0x006190FF); // Reset value
  ahb1lpenr.setGPIOALPEN(true); // Enable IO port A for Charging/USB plug/Keyboard pins
  ahb1lpenr.setGPIOBLPEN(true); // Enable IO port B for LED pins
  ahb1lpenr.setGPIOCLPEN(true); // Enable IO port C for LED/Keyboard pins
  ahb1lpenr.setGPIODLPEN(false); // Disable IO port D (LCD...)
  ahb1lpenr.setGPIOELPEN(true); // Enable IO port E for Keyboard/Battery pins
  ahb1lpenr.setGPIOFLPEN(false); // Disable IO port F
  ahb1lpenr.setGPIOGLPEN(false); // Disable IO port G
  ahb1lpenr.setGPIOHLPEN(false); // Disable IO port H
  ahb1lpenr.setGPIOILPEN(false); // Disable IO port I
  ahb1lpenr.setCRCLPEN(false);
  ahb1lpenr.setFLITFLPEN(false);
  ahb1lpenr.setSRAM1LPEN(false);
  ahb1lpenr.setDMA1LPEN(false);
  ahb1lpenr.setDMA2LPEN(false);
  RCC.AHB1LPENR()->set(ahb1lpenr);

  // AHB2 peripheral clock enable in low-power mode register
  class RCC::AHB2LPENR ahb2lpenr(0x000000C0); // Reset value
  ahb2lpenr.setOTGFSLPEN(false);
  ahb2lpenr.setRNGLPEN(false);
  RCC.AHB2LPENR()->set(ahb2lpenr);

  // AHB3 peripheral clock enable in low-power mode register
  class RCC::AHB3LPENR ahb3lpenr(0x00000003); // Reset value
  ahb3lpenr.setFMCLPEN(false);
  ahb3lpenr.setQSPILPEN(false);
  RCC.AHB3LPENR()->set(ahb3lpenr);

  // APB1 peripheral clock enable in low-power mode register
  class RCC::APB1LPENR apb1lpenr(0x17E6CDFF); // Reset value
  apb1lpenr.setTIM2LPEN(false);
  apb1lpenr.setTIM3LPEN(true); // Enable TIM3 in sleep mode for LEDs
  apb1lpenr.setTIM4LPEN(false);
  apb1lpenr.setTIM5LPEN(false);
  apb1lpenr.setTIM6LPEN(false);
  apb1lpenr.setTIM7LPEN(false);
  apb1lpenr.setTIM12LPEN(false);
  apb1lpenr.setTIM13LPEN(false);
  apb1lpenr.setTIM14LPEN(false);
  apb1lpenr.setRTCAPBLPEN(false);
  apb1lpenr.setWWDGLPEN(false);
  apb1lpenr.setSPI2LPEN(false);
  apb1lpenr.setSPI3LPEN(false);
  apb1lpenr.setUSART2LPEN(false);
  apb1lpenr.setUSART3LPEN(false);
  apb1lpenr.setI2C1LPEN(false);
  apb1lpenr.setI2C2LPEN(false);
  apb1lpenr.setI2C3LPEN(false);
  apb1lpenr.setCAN1LPEN(false);
  apb1lpenr.setPWRLPEN(false);
  apb1lpenr.setI2CFMP1LPEN(false);
  apb1lpenr.setCAN2LPEN(false);
  RCC.APB1LPENR()->set(apb1lpenr);

  // APB2 peripheral clock enable in low-power mode register
  class RCC::APB2LPENR apb2lpenr(0x0117F933); // Reset value
  apb2lpenr.setTIM1LPEN(false);
  apb2lpenr.setTIM8LPEN(false);
  apb2lpenr.setUSART1LPEN(false);
  apb2lpenr.setUSART6LPEN(false);
  apb2lpenr.setADC1LPEN(false);
  apb2lpenr.setSPI1LPEN(false);
  apb2lpenr.setSPI4LPEN(false);
  apb2lpenr.setSYSCFGLPEN(false);
  apb2lpenr.setTIM9LPEN(false);
  apb2lpenr.setTIM10LPEN(false);
  apb2lpenr.setTIM11LPEN(false);
  apb2lpenr.setSPI5LPEN(false);
  apb2lpenr.setSDIOLPEN(false);
  apb2lpenr.setEXTITEN(false);
  apb2lpenr.setDFSDM1LPEN(false);
  RCC.APB2LPENR()->set(apb2lpenr);
}

void shutdownClocks(bool keepLEDAwake) {
  // APB2 bus
  RCC.APB2ENR()->set(0x00008000); // Reset value

  // AHB2 bus
  RCC.AHB2ENR()->set(0); // Reset value

  // AHB3 bus
  RCC.AHB3ENR()->set(0); // Reset value

  // APB1
  class RCC::APB1ENR apb1enr(0x00000400); // Reset value
  // AHB1 bus
  class RCC::AHB1ENR ahb1enr(0); // Reset value
  if (keepLEDAwake) {
    apb1enr.setTIM3EN(true);
    ahb1enr.setGPIOBEN(true);
    ahb1enr.setGPIOCEN(true);
  }
  RCC.APB1ENR()->set(apb1enr);
  RCC.AHB1ENR()->set(ahb1enr);
}

}
}
}
