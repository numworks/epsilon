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

static constexpr int interruptionIndex[] = {6, 7, 8, 9, 10, 23, 28};

void initInterruptions() {
  // Init SVCall interrupt priorities
  // SVCall should be interruptible by EXTI
  CORTEX.SHPR2()->setSVCALL_PRI(20);
  CORTEX.SHPR3()->setPENDSV_PRI(10);
  // Init EXTI interrupts (corresponding to keyboard column pins)
  class NVIC::NVIC_ISER0 iser0(0); // Reset value
  class NVIC::NVIC_ICPR0 icpr0(0); // Reset value
  for (size_t i = 0; i < sizeof(interruptionIndex)/sizeof(int); i++) {
    iser0.setBit(interruptionIndex[i], true);
    icpr0.setBit(interruptionIndex[i], true);
  }
  // Flush pending interruptions
  NVIC.NVIC_ICPR0()->set(icpr0);
  // Enable interruptions
  NVIC.NVIC_ISER0()->set(iser0);
}

void shutdownInterruptions() {
  class NVIC::NVIC_ICER0 icer0(0); // Reset value
  for (size_t i = 0; i < sizeof(interruptionIndex)/sizeof(int); i++) {
    icer0.setBit(interruptionIndex[i], true);
  }
  // Disable interruptions
  NVIC.NVIC_ICER0()->set(icer0);
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

