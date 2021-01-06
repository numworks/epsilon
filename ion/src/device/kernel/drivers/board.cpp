#include "board.h"
#include <boot/isr.h>
#include <drivers/config/clocks.h>
#include <drivers/backlight.h>
#include <drivers/battery.h>
#include <drivers/display.h>
#include <drivers/external_flash.h>
#include <drivers/keyboard.h>
#include <drivers/led.h>
#include <drivers/swd.h>
#include <drivers/usb.h>
#include <kernel/drivers/timing.h>
#include <regs/regs.h>

typedef void(*ISR)(void);
extern ISR InitialisationVector[];

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void shutdown() {
  shutdownPeripherals();
  ExternalFlash::shutdown();
  shutdownPeripheralsClocks();
}

void shutdownCompensationCell() {
  SYSCFG.CMPCR()->setCMP_PD(false);
}

void initPeripherals(bool numworksAuthentication, bool fromBootloader) {
  Display::init();
  if (!fromBootloader) {
    Backlight::init();
  }
  Keyboard::init();
  LED::init(numworksAuthentication);
  Battery::init();
  if (!fromBootloader) {
    // USB has already be initialized by the bootloader
    USB::init();
  }
  SWD::init();
  Timing::init();
}

void shutdownPeripherals(bool keepLEDAwake) {
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

void initInterruptions() {
  // Init EXTI interrupts (corresponding to keyboard column pins)
  class NVIC::NVIC_ISER0 iser0(0); // Reset value
  iser0.setBit(6, true);
  iser0.setBit(7, true);
  iser0.setBit(8, true);
  iser0.setBit(9, true);
  iser0.setBit(10, true);
  iser0.setBit(23, true);
  NVIC.NVIC_ISER0()->set(iser0);

  // Init EXTI interrupt priorities
  /*
  class NVIC::NVIC_IPR ipr(0); // Reset value
  ipr.set(6, 0);
  ipr.set(7, 0);
  ipr.set(8, 0);
  ipr.set(9, 0);
  ipr.set(10, 0);
  ipr.set(23, 0);
  NVIC::NVIC_IPR()->set(ipr);
  */
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

}
}
}

