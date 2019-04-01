#include <drivers/board.h>
#include <drivers/cache.h>
#include <regs/regs.h>
#include <ion.h>

extern void * InitialisationVector;

// Public Ion methods

const char * Ion::fccId() {
  return "2ALWP-N0101";
}

// Private Ion::Device methods

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void initL1Cache() {
  Cache::enableICache();
  Cache::enableDCache();
}

void initMPU() {
  // Configure MPU settings for the FMC memory area
  // This is needed for interfacing with the LCD
  MPU.RNR()->setREGION(0x00);
  MPU.RBAR()->setADDR(0x60000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_32MB);
  MPU.RASR()->setXN(true);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setTEX(2);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  MPU.CTRL()->setPRIVDEFENA(true);
  MPU.CTRL()->setENABLE(true);
}

void init() {
  initFPU();
  initMPU();
  initClocks();

  // The bootloader leaves its own after flashing
  //SYSCFG.MEMRMP()->setMEM_MODE(SYSCFG::MEMRMP::MemMode::MainFlashmemory);
  // Ensure right location of interrupt vectors
  CM4.VTOR()->setVTOR((void*)&InitialisationVector);

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

  initPeripherals();
  // Initiate L1 cache after initiating the external flash
  initL1Cache();

  // TODO if EPSILON_DEVICE_BENCH, run bench? See n0100
}

void initClocks() {
  /* System clock
   * Configure the CPU at 192 MHz and USB at 48 MHz. */

  /* After reset, the device is using the high-speed internal oscillator (HSI)
   * as a clock source, which runs at a fixed 16 MHz frequency. The HSI is not
   * accurate enough for reliable USB operation, so we need to use the external
   * high-speed oscillator (HSE). */

  // Enable the HSI and wait for it to be ready
  RCC.CR()->setHSION(true);
  while(!RCC.CR()->getHSIRDY()) {
  }

  // Enable the HSE and wait for it to be ready
  RCC.CR()->setHSEON(true);
  while(!RCC.CR()->getHSERDY()) {
  }

  // Enable PWR peripheral clock
  RCC.APB1ENR()->setPWREN(true);
  // Choose Voltage scale 1
  PWR.CR()->setVOS(PWR::CR::Voltage::Scale1);
  //while (!PWR.CSR1()->getVOSRDY()) {}

  /* Given the crystal used on our device, the HSE will oscillate at 8 MHz. By
   * piping it through a phase-locked loop (PLL) we can derive other frequencies
   * for use in different parts of the system.  */

  // Configure the PLL ratios and use HSE as a PLL input
  RCC.PLLCFGR()->setPLLM(8);
  RCC.PLLCFGR()->setPLLN(384);
  RCC.PLLCFGR()->setPLLQ(8);
  RCC.PLLCFGR()->setPLLSRC(RCC::PLLCFGR::PLLSRC::HSE);

  /* If you want to considerably slow down the whole machine uniformely, which
   * can be very useful to diagnose performance issues, just uncomment the line
   * below. Note that even booting takes a few seconds, so don't be surprised
   * if the screen is black for a short while upon booting. */
  //RCC.CFGR()->setHPRE(RCC::CFGR::AHBPrescaler::SysClkDividedBy2);

  // Enable the PLL and wait for it to be ready
  RCC.CR()->setPLLON(true);

  PWR.CR()->setODEN(true);
  while(!PWR.CSR1()->getODRDY()) {
  }

  PWR.CR()->setODSWEN(true);
  while(!PWR.CSR1()->getODSWRDY()) {
  }

  /* After reset the Flash runs as fast as the CPU. When we clock the CPU faster
   * the flash memory cannot follow and therefore flash memory accesses need to
   * wait a little bit.
   * The spec tells us that at 2.8V and over 210MHz the flash expects 7 WS. */
  FLASH.ACR()->setLATENCY(7);

  /* Enable prefetching flash instructions */
  /* Fetching instructions increases slightly the power consumption but the
   * increase is negligible compared to the screen consumption. */
  FLASH.ACR()->setPRFTEN(true);

  /* Enable the ART */
  FLASH.ACR()->setARTEN(true);

  // 192 MHz is too fast for APB1. Divide it by four to reach 48 MHz
  RCC.CFGR()->setPPRE1(RCC::CFGR::APBPrescaler::AHBDividedBy4);
  // 192 MHz is too fast for APB2. Divide it by two to reach 96 MHz
  RCC.CFGR()->setPPRE2(RCC::CFGR::APBPrescaler::AHBDividedBy2);

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
  class RCC::AHB1ENR ahb1enr(0x00100000); // Reset value
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

  // APB2 bus
  class RCC::APB2ENR apb2enr(0); // Reset value
  apb2enr.setADC1EN(true);
  apb2enr.setSYSCFGEN(true);
  apb2enr.setUSART6EN(true); // TODO if EPSILON_DEVICE_BENCH?
  RCC.APB2ENR()->set(apb2enr);
}

void shutdownClocks(bool keepLEDAwake) {
  // APB2 bus
  RCC.APB2ENR()->set(0); // Reset value

  // APB1
  class RCC::APB1ENR apb1enr(0); // Reset value
  // AHB1 bus
  class RCC::AHB1ENR ahb1enr(0x00100000); // Reset value
  if (keepLEDAwake) {
    apb1enr.setTIM3EN(true);
    ahb1enr.setGPIOBEN(true);
  }
  RCC.APB1ENR()->set(apb1enr);
  RCC.AHB1ENR()->set(ahb1enr);

  RCC.AHB3ENR()->setFSMCEN(false);
}

}
}
}
