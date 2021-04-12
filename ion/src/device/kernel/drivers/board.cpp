#include "board.h"
#include <boot/isr.h>
#include <drivers/config/clocks.h>
#include <drivers/backlight.h>
#include <drivers/battery.h>
#include <drivers/display.h>
#include <drivers/events.h>
#include <drivers/keyboard.h>
#include <drivers/led.h>
#include <drivers/reset.h>
#include <drivers/swd.h>
#include <drivers/timing.h>
#include <drivers/usb_privileged.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/timing.h>
#include <kernel/warning_display.h>
#include <ion/src/shared/platform_info.h>
#include <regs/regs.h>
#include <shared/drivers/config/board.h>

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
  Display::init();
  if (!fromBootloader) {
    Backlight::init();
  }
  Keyboard::init();
  LED::init();
  Battery::init();
  if (!fromBootloader) {
    // USB has already be initialized by the bootloader
    USB::init();
  }
  SWD::init();
  Timing::init();
  Events::init();
}

void shutdownPeripherals(bool keepLEDAwake) {
  Events::shutdown();
  Timing::shutdown();
  SWD::shutdown();
  USB::shutdown();
  Battery::shutdown();
  if (!keepLEDAwake) {
    LED::shutdown();
  }
  Keyboard::shutdown();
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

// TODO: both initInterruptions/shutdownInterruptions are not required yet. Remove?
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

uint32_t userlandStart() {
  return isRunningSlotA() ? slotAUserlandStart() : slotBUserlandStart();
}

uint32_t switchExecutableSlot() {
  assert(Authentication::trustedUserland());
  PlatformInfo * platformInfoA = reinterpret_cast<PlatformInfo *>(slotAUserlandStart() + Ion::Device::Board::Config::UserlandHeaderOffset);
  PlatformInfo * platformInfoB = reinterpret_cast<PlatformInfo *>(slotBUserlandStart() + Ion::Device::Board::Config::UserlandHeaderOffset);
  int deltaKernelVersion = platformInfoA->kernelVersionValue() - platformInfoB->kernelVersionValue();
  int deltaUserlandVersion = platformInfoA->epsilonVersionValue() - platformInfoB->epsilonVersionValue();
  // Delta = newVersion - oldVersion
  bool slotARunning = isRunningSlotA();
  if (slotARunning) {
    deltaKernelVersion = -deltaKernelVersion;
    deltaUserlandVersion = -deltaUserlandVersion;
  }
  bool otherSlotUserlandAuthentication = Authentication::userlandTrust(!slotARunning);
  if (deltaKernelVersion < 0 || (otherSlotUserlandAuthentication && deltaUserlandVersion < 0)) {
    WarningDisplay::obsoleteSoftware();
    Ion::Timing::msleep(5000);
    return 0;
  }
  if (!otherSlotUserlandAuthentication && deltaKernelVersion > 0) {
    WarningDisplay::kernelUpgradeRequired();
    Ion::Timing::msleep(5000);
    return 0;
  }
  if (otherSlotUserlandAuthentication) {
    assert(deltaKernelVersion >= 0 && deltaUserlandVersion >= 0);
    Reset::coreWhilePlugged();
  } else {
    assert(deltaKernelVersion == 0);
    Authentication::updateTrust(false);
    WarningDisplay::unauthenticatedUserland();
    Ion::Timing::msleep(3000);
  }
  return slotARunning ? slotBUserlandStart() : slotAUserlandStart();
}

void downgradeTrustLevel() {
  LED::setColor(KDColorBlack);
}

bool addressInUserlandRAM(void * address) {
  uint32_t a = reinterpret_cast<uint32_t>(address);
  return a >= Config::UserlandSRAMAddress && a < Config::UserlandSRAMAddress + Config::UserlandSRAMLength;
}

}
}
}
