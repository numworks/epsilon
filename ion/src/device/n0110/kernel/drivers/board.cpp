#include <drivers/cache.h>
#include <drivers/config/clocks.h>
#include <drivers/internal_flash.h>
#include <drivers/reset.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/board.h>
#include <kernel/warning_display.h>
#include <regs/regs.h>
#include <shared/drivers/config/board.h>
#include <shared/drivers/config/external_flash.h>

typedef void(*ISR)(void);
extern ISR InitialisationVector[];

extern "C" {
  extern char _isr_vector_table_start_flash;
}

// Private Ion::Device methods

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void initMPU() {
  /* Reconfigure the MPU for kernel+userland */
  // 1. Disable the MPU
  // 1.1 Memory barrier
  Cache::dmb();

  // 1.3 Disable the MPU and clear the control register
  MPU.CTRL()->setENABLE(false);

  // 2. MPU settings
  int sector = 0;

  /* We will activate the default memory map as the background region which is
   * accessible from privileged software only (PM0253). */

  /* 2.1. The bootloader sector 0x0200 0000 - 0x1FFF FFFF is read-only. The
   * userland requires accesses to:
   * - the trampoline section and all its dependencies within the internal flash
   * - the electronic signature located at 0x1FF0 7A10
   * A collateral effect is to give access to the Option bytes but that's not
   * really an issue in Read-only mode. The region size is too large but
   * overlapped by the next region.
   * TODO: could we merge the section and the next one?
   * */
  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(Board::Config::BootloaderStartAddress);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_512MB);
  MPU.RASR()->setSRD(0);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RO);
  MPU.RASR()->setXN(false);
  MPU.RASR()->setTEX(0);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(1);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  /* 2.2 Enable unprivileged access to the SRAM - except the range dedicated to
   * the kernel */
  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(Board::Config::UserlandSRAMAddress);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_256KB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setXN(false);
  MPU.RASR()->setTEX(1);
  MPU.RASR()->setS(1);
  MPU.RASR()->setC(1);
  MPU.RASR()->setB(1);
  MPU.RASR()->setENABLE(true);

  // 2.3 Forbid unprivileged access to kernel RAM
  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(Board::Config::KernelRAMAddress);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_4KB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::PrivilegedRW);
  MPU.RASR()->setXN(false);
  MPU.RASR()->setTEX(1);
  MPU.RASR()->setS(1);
  MPU.RASR()->setC(1);
  MPU.RASR()->setB(1);
  MPU.RASR()->setENABLE(true);

  // 2.4 Enable unprivileged access to the OTG registers
  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(OTG.Base());
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_256KB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setXN(true);
  MPU.RASR()->setTEX(2);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  // 2.5 Configure a MPU region for the FMC memory area
  /* This is needed for interfacing with the LCD
   * Actually, we only need to access the FMC command and data addresses (32
   * bits respectively at 0x60000000 and 0x60020000). However, we can only
   * configure 8 MPU regions so to save an extra sector we set all 256MB as
   * readable/writable non-cachable, non-buffereable and non shareable. */

  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x60000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_256MB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::PrivilegedRW);
  MPU.RASR()->setXN(true);
  MPU.RASR()->setTEX(2);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  // 2.6 Configure MPU regions for the QUADSPI peripheral
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

  /* 2.7 Enable unpriveleged access to ITCM RAM
   * This memory is only used by external application. */
  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(Board::Config::ITCMRAMAdress);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_16KB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setXN(false);
  MPU.RASR()->setTEX(1);
  MPU.RASR()->setS(1);
  MPU.RASR()->setC(1);
  MPU.RASR()->setB(1);
  MPU.RASR()->setENABLE(true);

  /* We assert that all sectors have been initialized. Otherwise, the bootloader
   * configuration is still active on the last sectors when their configuration
   * should be reset. */
  assert(sector == 8);

  // 3. Enable default memory map
  MPU.CTRL()->setPRIVDEFENA(true);
  // 4. Enable MPU
  MPU.CTRL()->setENABLE(true);

  // 5. Data/instruction synchronisation barriers to ensure that the new MPU configuration is used by subsequent instructions.
  Cache::dsb();
  Cache::isb();
}

void init() {
  initMPU();
  initPeripheralsClocks();
  initInterruptionPriorities();
  // Ensure right location of interrupt vectors
  CORTEX.VTOR()->setVTOR((void*)&InitialisationVector);
  Cache::enable();
}

void initPeripheralsClocks() {
  /* Some clocks might have already be init by the bootloader:
   * - AHB1 (GPIOs)
   * - AHB2 (OTGFSEN)
   * - AHB3 (FSMCEN + QSPI) */

  // APB1 bus
  // We're using TIM2 for the events
  RCC.APB1ENR()->setTIM2EN(true);
  // We're using TIM3 for the LEDs
  RCC.APB1ENR()->setTIM3EN(true);
  // We're using TIM4 and TIM5 for keyboard
  RCC.APB1ENR()->setTIM4EN(true);
  RCC.APB1ENR()->setTIM5EN(true);
  // We're using TIM6 for the circuit breaker
  RCC.APB1ENR()->setTIM6EN(true);

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

void shutdownPeripheralsClocks(bool keepLEDAwake) {
  // APB2 bus
  RCC.APB2ENR()->set(0); // Reset value

  // AHB2 bus
  RCC.AHB2ENR()->set(0); // Reset value

  // AHB3 bus
  class RCC::AHB3ENR ahb3enr(0); // Reset value
  // Required by external flash
  ahb3enr.setQSPIEN(true);
  RCC.AHB3ENR()->set(ahb3enr); // Reset value

  // APB1
  class RCC::APB1ENR apb1enr(0); // Reset value
  // AHB1 bus
  class RCC::AHB1ENR ahb1enr(0x00100000); // Reset value
  // GPIO B, C, D, E are used the by external flash
  ahb1enr.setGPIOBEN(true);
  ahb1enr.setGPIOCEN(true);
  ahb1enr.setGPIODEN(true);
  ahb1enr.setGPIOEEN(true);
  if (keepLEDAwake) {
    apb1enr.setTIM3EN(true);
    // GPIO B is already on
    ahb1enr.setGPIOBEN(true);
  }
  RCC.APB1ENR()->set(apb1enr);
  RCC.AHB1ENR()->set(ahb1enr);
}

bool isRunningSlotA() {
  return reinterpret_cast<uint32_t>(&_isr_vector_table_start_flash) < Board::Config::SlotBStartAddress;
}

bool isInReflashableSector(uint32_t address) {
  if (isRunningSlotA()) {
    return address >= Board::Config::SlotBStartAddress && address < ExternalFlash::Config::EndAddress;
  }
  return (address >= ExternalFlash::Config::StartAddress && address < Board::Config::SlotBStartAddress) || (address >= Board::Config::StorageStartAdress && address < ExternalFlash::Config::EndAddress);
}

void switchExecutableSlot(uint32_t leaveAddress) {
  assert(Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::NumWorks);
  if (!isInReflashableSector(leaveAddress)) {
    return Reset::core();
  }
  KernelHeader * currentInfo = kernelHeader();
  UserlandHeader * otherInfo = reinterpret_cast<UserlandHeader *>(leaveAddress);

  if (!otherInfo->isValid()) {
    // Can't get any information on the kernel version required
    return Reset::core();
  }
  int deltaVersion = currentInfo->epsilonVersionComparedTo(otherInfo->expectedEpsilonVersion());
  if (deltaVersion != 0) {
    WarningDisplay::upgradeRequired();
    return Reset::core();
  } else {
    Authentication::downgradeClearanceLevelTo(Ion::Authentication::ClearanceLevel::ThirdParty);
    WarningDisplay::unauthenticatedUserland(); // UNOFFICIAL SOFTWARE
  }
}

}
}
}
