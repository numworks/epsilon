#include "board.h"
#include <boot/isr.h>
#include <drivers/config/clocks.h>
#include <drivers/battery.h>
#include <drivers/display.h>
#include <drivers/events.h>
#include <drivers/led.h>
#include <drivers/reset.h>
#include <drivers/swd.h>
#include <drivers/timing.h>
#include <drivers/usb_privileged.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/backlight.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/timing.h>
#include <kernel/warning_display.h>
#include <regs/regs.h>
#include <shared/drivers/config/board.h>
#include <shared/drivers/usb.h>

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void shutdown() {
  shutdownPeripherals();
  shutdownPeripheralsClocks();
}

void shutdownCompensationCell() {
  SYSCFG.CMPCR()->setCMP_PD(false);
}

void initPeripherals(bool fromBootloader) {
  if (!fromBootloader) {
    Display::init();
    Backlight::init();
    Battery::init();
    USB::init();
  }
  Keyboard::init(fromBootloader);
  LED::init();
  SWD::init();
  Timing::init();
  Events::init();
}

void shutdownPeripherals(bool keepLEDAwake) {
  Events::shutdown();
  Timing::shutdown();
  SWD::shutdown();
  if (!keepLEDAwake) {
    LED::shutdown();
  }
  Keyboard::shutdown();
  USB::shutdown();
  Battery::shutdown();
  Backlight::shutdown();
  Display::shutdown();
  shutdownCompensationCell();
}

void initInterruptionPriorities() {
  // Init SVCall interrupt priorities
  // SVCall should be interruptible by EXTI/systick/Timers interruptions
  CORTEX.SHPR2()->setSVCALL_PRI(static_cast<uint8_t>(NVIC::NVIC_IPR::InterruptionPriority::Medium));
  // pendSV has the lowest priority to be executed when no other interruptions are queued
  CORTEX.SHPR3()->setPENDSV_PRI(static_cast<uint8_t>(NVIC::NVIC_IPR::InterruptionPriority::Low));
}

void initInterruptions() {
  Keyboard::initInterruptions();
  Events::initInterruptions();
  Timing::initInterruptions();
}

void shutdownInterruptions() {
  Timing::shutdownInterruptions();
  Events::shutdownInterruptions();
  Keyboard::shutdownInterruptions();
}

static Frequency sStandardFrequency = Frequency::High;

void setStandardFrequency(Frequency f) {
  sStandardFrequency = f;
  setClockStandardFrequency();
}

void setClockLowFrequency() {
  // TODO: Update TIM3 prescaler or ARR to avoid irregular LED blinking
  // Change the systick frequency to compensate the HCLK frequency change
  Device::Timing::setSysTickFrequency(Ion::Device::Clocks::Config::HCLKLowFrequency);
  RCC.CFGR()->setHPRE(Clocks::Config::AHBLowFrequencyPrescalerReg);
}

void setClockStandardFrequency() {
  if (sStandardFrequency == Frequency::Low) {
    return setClockLowFrequency();
  }
  assert(sStandardFrequency == Frequency::High);
  // TODO: Update TIM3 prescaler or ARR to avoid irregular LED blinking
  // Change the systick frequency to compensate the KCLK frequency change
  RCC.CFGR()->setHPRE(RCC::CFGR::AHBPrescaler::SysClk);
  Device::Timing::setSysTickFrequency(Ion::Device::Clocks::Config::HCLKFrequency);
}

void switchExecutableSlot(uint32_t leaveAddress) {
  assert(Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::NumWorks);
  if (!isInReflashableSector(leaveAddress)) {
    return Reset::coreWhilePlugged();
  }
  /* In N0100, the userland has been overriden, we have to extract the
   * platform information from the slot information kept in RAM. */
  KernelHeader * currentInfo = USB::slotInfo()->kernelHeader();
  UserlandHeader * otherInfo = reinterpret_cast<UserlandHeader *>(leaveAddress);

  if (!otherInfo->isValid()) {
    // Can't get any information on the kernel version required
    return Reset::coreWhilePlugged();
  }
  int deltaVersion = currentInfo->epsilonVersionComparedTo(otherInfo->expectedEpsilonVersion());
  if (deltaVersion != 0) {
    WarningDisplay::upgradeRequired();
    return Reset::coreWhilePlugged();
  } else {
    Authentication::downgradeClearanceLevelTo(Ion::Authentication::ClearanceLevel::ThirdParty);
    WarningDisplay::unauthenticatedUserland(); // UNOFFICIAL SOFTWARE
  }
}

void enableExternalApps() {
  WarningDisplay::externalAppsAvailable();
  Authentication::downgradeClearanceLevelTo(Ion::Authentication::ClearanceLevel::NumWorksAndThirdPartyApps);
}

bool addressInUserlandRAM(void * address) {
  uint32_t a = reinterpret_cast<uint32_t>(address);
  return a >= Config::UserlandSRAMAddress && a < Config::UserlandSRAMAddress + Config::UserlandSRAMLength;
}

void lockUnlockedPCBVersion() {
  if (pcbVersionIsLocked()) {
    return;
  }
  /* PCB version is unlocked : the device is a N0110 that has been
   * produced prior to the pcb revision 3.43. */
  PCBVersion version = pcbVersion();
  if (version != 0) {
    /* Some garbage has been written in OTP0. We overwrite it fully, which is
     * interepreted as blank. */
    writePCBVersion(k_alternateBlankVersion);
  }
  lockPCBVersion();
}

}
}
}
