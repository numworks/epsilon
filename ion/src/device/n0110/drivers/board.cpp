#include <drivers/board.h>
#include <drivers/cache.h>
#include <drivers/config/clocks.h>
#include <drivers/external_flash.h>
#include <regs/regs.h>
#include <ion.h>

typedef void(*ISR)(void);
extern ISR InitialisationVector[];

// Public Ion methods

const char * Ion::fccId() {
  return "2ALWP-N0110";
}

// Private Ion::Device methods

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void initMPU() {
  // 1. Disable the MPU
  // 1.1 Memory barrier
  Cache::dmb();

  // 1.2 Disable fault exceptions
  CORTEX.SHCRS()->setMEMFAULTENA(false);

  // 1.3 Disable the MPU and clear the control register
  MPU.CTRL()->setENABLE(false);

  // 2. MPU settings
  // 2.1 Configure a MPU region for the FMC memory area
  /* This is needed for interfacing with the LCD
   * We define the whole FMC memory bank 1 as strongly ordered, non-executable
   * and not accessible. We define the FMC command and data addresses as
   * writeable non-cachable, non-buffereable and non shareable. */
  int sector = 0;
  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x60000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_256MB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::NoAccess);
  MPU.RASR()->setXN(true);
  MPU.RASR()->setTEX(2);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x60000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_32B);
  MPU.RASR()->setXN(true);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setTEX(2);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x60000000+0x20000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_32B);
  MPU.RASR()->setXN(true);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setTEX(2);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  // 2.2 Configure MPU regions for the QUADSPI peripheral
  /* L1 Cache can issue speculative reads to any memory address. But, when the
   * Quad-SPI is in memory-mapped mode, if an access is made to an address
   * outside of the range defined by FSIZE but still within the 256Mbytes range,
   * then an AHB error is given (AN4760). To prevent this to happen, we
   * configure the MPU to define the whole Quad-SPI addressable space as
   * strongly ordered, non-executable and not accessible. Plus, we define the
   * Quad-SPI region corresponding to the Expternal Chip as executable and
   * fully accessible (AN4861). */
  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x90000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_256MB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::NoAccess);
  MPU.RASR()->setXN(true);
  MPU.RASR()->setTEX(0);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x90000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_8MB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setXN(false);
  MPU.RASR()->setTEX(0);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(1);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  // 2.3 Enable MPU
  MPU.CTRL()->setPRIVDEFENA(true);
  MPU.CTRL()->setENABLE(true);

  // 3. Data/instruction synchronisation barriers to ensure that the new MPU configuration is used by subsequent instructions.
  Cache::dsb();
  Cache::isb();
}

void init() {
  initFPU();
  initMPU();
  initClocks();

  // The bootloader leaves its own after flashing
  //SYSCFG.MEMRMP()->setMEM_MODE(SYSCFG::MEMRMP::MemMode::MainFlashmemory);
  // Ensure right location of interrupt vectors
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

  ExternalFlash::init();
  // Initiate L1 cache after initiating the external flash
  Cache::enable();
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

  /* To pass electromagnetic compatibility tests, we activate the Spread
   * Spectrum clock generation, which adds jitter to the PLL clock in order to
   * "lower peak-energy on the central frequency" and its harmonics.
   * It must be done before enabling the PLL. */
  class RCC::SSCGR sscgr(0); // Reset value
  sscgr.setMODPER(Clocks::Config::SSCG_MODPER);
  sscgr.setINCSTEP(Clocks::Config::SSCG_INCSTEP);
  sscgr.setSPREADSEL(RCC::SSCGR::SPREADSEL::CenterSpread);
  sscgr.setSSCGEN(true);
  RCC.SSCGR()->set(sscgr);

  /* Given the crystal used on our device, the HSE will oscillate at 8 MHz. By
   * piping it through a phase-locked loop (PLL) we can derive other frequencies
   * for use in different parts of the system. */

  // Configure the PLL ratios and use HSE as a PLL input
  RCC.PLLCFGR()->setPLLM(Clocks::Config::PLL_M);
  RCC.PLLCFGR()->setPLLN(Clocks::Config::PLL_N);
  RCC.PLLCFGR()->setPLLQ(Clocks::Config::PLL_Q);
  RCC.PLLCFGR()->setPLLSRC(RCC::PLLCFGR::PLLSRC::HSE);

  // Enable the PLL and wait for it to be ready
  RCC.CR()->setPLLON(true);

  // Enable Over-drive
  PWR.CR()->setODEN(true);
  while(!PWR.CSR()->getODRDY()) {
  }

  PWR.CR()->setODSWEN(true);
  while(!PWR.CSR()->getODSWRDY()) {
  }

  // Choose Voltage scale 1
  PWR.CR()->setVOS(PWR::CR::Voltage::Scale1);
  while (!PWR.CSR()->getVOSRDY()) {}

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
  RCC.CFGR()->setPPRE1(Clocks::Config::APB1PrescalerReg);
  // 192 MHz is too fast for APB2. Divide it by two to reach 96 MHz
  RCC.CFGR()->setPPRE2(Clocks::Config::APB2PrescalerReg);

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
  apb2enr.setUSART6EN(true); // TODO required if building bench target only?
  RCC.APB2ENR()->set(apb2enr);

  // Configure clocks in sleep mode
  // AHB1 peripheral clock enable in low-power mode register
  class RCC::AHB1LPENR ahb1lpenr(0x7EF7B7FF); // Reset value
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
  ahb1lpenr.setAXILPEN(false);
  ahb1lpenr.setSRAM2LPEN(false);
  ahb1lpenr.setBKPSRAMLPEN(false);
  ahb1lpenr.setDTCMLPEN(false);
  ahb1lpenr.setOTGHSLPEN(false);
  ahb1lpenr.setOTGHSULPILPEN(false);
  RCC.AHB1LPENR()->set(ahb1lpenr);

  // AHB2 peripheral clock enable in low-power mode register
  class RCC::AHB2LPENR ahb2lpenr(0x000000F1); // Reset value
  ahb2lpenr.setOTGFSLPEN(false);
  ahb2lpenr.setRNGLPEN(false);
  ahb2lpenr.setAESLPEN(false);
  RCC.AHB2LPENR()->set(ahb2lpenr);

  // AHB3 peripheral clock enable in low-power mode register
  class RCC::AHB3LPENR ahb3lpenr(0x00000003); // Reset value
  ahb3lpenr.setFMCLPEN(false);
  ahb3lpenr.setQSPILPEN(false);
  RCC.AHB3LPENR()->set(ahb3lpenr);

  // APB1 peripheral clock enable in low-power mode register
  class RCC::APB1LPENR apb1lpenr(0xFFFFCBFF); // Reset value
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
  apb1lpenr.setLPTIM1LPEN(false);
  apb1lpenr.setUSART4LPEN(false);
  apb1lpenr.setUSART5LPEN(false);
  apb1lpenr.setOTGHSLPEN(false);
  apb1lpenr.setOTGHSULPILPEN(false);
  RCC.APB1LPENR()->set(apb1lpenr);

  // APB2 peripheral clock enable in low-power mode register
  class RCC::APB2LPENR apb2lpenr(0x04F77F33); // Reset value
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
  apb2lpenr.setSDMMC2LPEN(false);
  apb2lpenr.setADC2LPEN(false);
  apb2lpenr.setADC3LPEN(false);
  apb2lpenr.setSAI1LPEN(false);
  apb2lpenr.setSAI2LPEN(false);
  RCC.APB2LPENR()->set(apb2lpenr);
}

void shutdownClocks(bool keepLEDAwake) {
  // APB2 bus
  RCC.APB2ENR()->set(0); // Reset value

  // AHB2 bus
  RCC.AHB2ENR()->set(0); // Reset value

  // AHB3 bus
  RCC.AHB3ENR()->set(0); // Reset value

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
}

}
}
}
