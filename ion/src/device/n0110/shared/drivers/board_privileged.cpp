#include <drivers/board_privileged.h>
#include <drivers/config/board.h>
#include <drivers/config/clocks.h>
#include <drivers/internal_flash_otp.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void initSystemClocks() {
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

  class PWR::CR pwr_cr(0x0000C000); // Reset value
  // Under-drive enabled in STOP mode
  pwr_cr.setUDEN(PWR::CR::UnderDrive::Enable);
  // Choose Voltage scale 1
  pwr_cr.setVOS(PWR::CR::Voltage::Scale1);
  PWR.CR()->set(pwr_cr);

  while (!PWR.CSR()->getVOSRDY()) {}

  // Enable Over-drive
  // NB: ODEN & ODSWEN can't be set simultaneously
  PWR.CR()->setODEN(true);
  while(!PWR.CSR()->getODRDY()) {
  }

  PWR.CR()->setODSWEN(true);
  while(!PWR.CSR()->getODSWRDY()) {
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
}

constexpr int k_pcbVersionOTPBlock = 0;
constexpr int k_pcbVersionOTPIndex = 0;

/* As we want the PCB versions to be in ascending order chronologically, and
 * because the OTP are initialized with 1s, we store the bitwise-not of the
 * version number. This way, devices with blank OTP are considered version 0. */

PCBVersion pcbVersion() {
#if IN_FACTORY
  /* When flashing for the first time, we want all systems that depend on the
   * PCB version to function correctly before flashing the PCB version. This
   * way, flashing the PCB version can be done last. */
  return PCB_LATEST;
#else
  PCBVersion version = readPCBVersionInMemory();
  return (version == k_alternateBlankVersion ? 0 : version);
#endif
}

PCBVersion readPCBVersionInMemory() {
  return ~static_cast<PCBVersion>(InternalFlash::readOTPAtIndex(k_pcbVersionOTPBlock, k_pcbVersionOTPIndex));
}

void writePCBVersion(PCBVersion version) {
  PCBVersion formattedVersion = ~version;
  InternalFlash::writeOTPAtIndex(k_pcbVersionOTPBlock, k_pcbVersionOTPIndex, static_cast<uint32_t>(formattedVersion));
}

void lockPCBVersion() {
  InternalFlash::lockOTPAtBlockIndex(k_pcbVersionOTPBlock);
}

bool pcbVersionIsLocked() {
  return InternalFlash::isLockedOTPAtBlockIndex(k_pcbVersionOTPBlock);
}

}
}
}
