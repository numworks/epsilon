#include "board.h"
#include <drivers/config/clocks.h>
#include <drivers/config/internal_flash.h>
#include <drivers/backlight.h>
#include <drivers/cache.h>
#include <drivers/display.h>
#include <drivers/internal_flash.h>
#include <drivers/internal_flash_otp.h>
#include <drivers/usb_privileged.h>
#include <drivers/external_flash.h>
#include <regs/regs.h>

typedef void(*ISR)(void);
extern ISR InitialisationVector[];

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

// shared/drivers/board.h

void init() {
  // MPU configuration is required to use the external flash
  initMPU();
  initSystemClocks();
  initPeripheralsClocks();
  CORTEX.VTOR()->setVTOR((void*)&InitialisationVector);

  // GPIO default states before initializing External flash
  setDefaultGPIO();

  initPeripherals();
}

void shutdown() {
  shutdownPeripherals();
  shutdownPeripheralsClocks();
}

void setClockLowFrequency() {
  RCC.CFGR()->setHPRE(Clocks::Config::AHBLowFrequencyPrescalerReg);
}

void setClockStandardFrequency() {
  RCC.CFGR()->setHPRE(RCC::CFGR::AHBPrescaler::SysClk);
}

// flasher/drivers/board.h

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

void initPeripherals() {
  initCompensationCell();
  Backlight::init();
  USB::init();
  Display::init();
  ExternalFlash::init();
}

void shutdownPeripherals() {
  ExternalFlash::shutdown();
  Display::shutdown();
  USB::shutdown();
  Backlight::shutdown();
  shutdownCompensationCell();
}

void initPeripheralsClocks() {
  // AHB1 bus
  class RCC::AHB1ENR ahb1enr(0x00100000); // Reset value
  /* We activate clocks of all GPIO groups to be able to configurate their
   * default states in the bootloader.
   * Also:
   * - LEDs are using GPIO B
   * - USB requires GPIO A and C
   * - External flash needs GPIO B, C, D, E
   * - Backlight needs GPIO E
   * - Display needs GPIO D, E
   */
  ahb1enr.setGPIOAEN(true);
  ahb1enr.setGPIOBEN(true);
  ahb1enr.setGPIOCEN(true);
  ahb1enr.setGPIODEN(true);
  ahb1enr.setGPIOEEN(true);
#if USE_DMA
  ahb1enr.setDMA2EN(true);
#endif
  RCC.AHB1ENR()->set(ahb1enr);

  // AHB2 bus
  RCC.AHB2ENR()->setOTGFSEN(true);

  // AHB3 bus (display)
  RCC.AHB3ENR()->setFSMCEN(true);
  RCC.AHB3ENR()->setQSPIEN(true);

  // APB2 bus
  RCC.APB2ENR()->setSYSCFGEN(true);
}

void shutdownPeripheralsClocks() {
  // AHB1 bus
  RCC.AHB1ENR()->set(0x00100000); // Reset value

  // AHB2 bus
  RCC.AHB2ENR()->set(0); // Reset value

  // AHB3 bus
  RCC.AHB3ENR()->setQSPIEN(0); // Reset value

  // APB2 bus
  RCC.APB2ENR()->setSYSCFGEN(false);
}

void initInterruptions() {}

}
}
}
