#include <regs/regs.h>
#include <drivers/cache.h>

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
  int sector = 0;

  // PM0253 says "The background region has the same memory access attributes as the default memory map, but is accessible from privileged software only."
  // We configure the internal flash to be accessible even by unpriviledge software
  // https://interrupt.memfault.com/blog/fix-bugs-and-secure-firmware-with-the-mpu
  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x00000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_1GB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setXN(false);
  MPU.RASR()->setTEX(0);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(1);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x20000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_512MB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setXN(false);
  MPU.RASR()->setTEX(0);
  MPU.RASR()->setS(1);
  MPU.RASR()->setC(1);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x40000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_1GB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setXN(false);
  MPU.RASR()->setTEX(0);
  MPU.RASR()->setS(1);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(1);
  MPU.RASR()->setENABLE(true);
  // Subregion??? for LED ???


  // 2.1 Configure a MPU region for the FMC memory area
  /* This is needed for interfacing with the LCD
   * We define the whole FMC memory bank 1 as strongly ordered, non-executable
   * and not accessible. We define the FMC command and data addresses as
   * writeable non-cachable, non-buffereable and non shareable. */
  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x60000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_256MB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setXN(true);
  MPU.RASR()->setTEX(2);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  /*MPU.RNR()->setREGION(sector++);
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
  MPU.RASR()->setENABLE(true);*/

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

}
}
}


