#include <ion/led.h>
#include <ion/display.h>
#include "led.h"
#include "regs/regs.h"

// Public Ion::LED methods

void Ion::LED::setColor(KDColor c) {
  TIM3.CCR1()->set(Device::dutyCycleForUInt8(c.red()));
  TIM3.CCR2()->set(Device::dutyCycleForUInt8(c.green()));
  TIM3.CCR3()->set(Device::dutyCycleForUInt8(c.blue()));
}

// We're also implementing backlight control

void Ion::Display::setBacklightIntensity(uint8_t intensity) {
  TIM3.CCR4()->set(Ion::LED::Device::dutyCycleForUInt8(intensity));
}

// Private Ion::Device::LED methods

namespace Ion {
namespace LED {
namespace Device {

void init() {
  initGPIO();
  initTimer();
}

void initGPIO() {
  /* RED_LED(PA6), GREEN_LED(PA7), BLUE_LED(PB0), LCD_LED(PC9) are driven using
   * a timer, which is an alternate function. */
  GPIOA.MODER()->setMode(6, GPIO::MODER::Mode::AlternateFunction);
  GPIOA.MODER()->setMode(7, GPIO::MODER::Mode::AlternateFunction);
  GPIOB.MODER()->setMode(0, GPIO::MODER::Mode::AlternateFunction);
  GPIOC.MODER()->setMode(9, GPIO::MODER::Mode::AlternateFunction);

  /* More precisely, we will use AF02, which maps PA6 to TIM3_CH1, PA7 to
   * TIM3_CH2, PB0 to TIM3_CH3 and PC9 to TIM3_CH4. */
  GPIOA.AFR()->setAlternateFunction(6, GPIO::AFR::AlternateFunction::AF2);
  GPIOA.AFR()->setAlternateFunction(7, GPIO::AFR::AlternateFunction::AF2);
  GPIOB.AFR()->setAlternateFunction(0, GPIO::AFR::AlternateFunction::AF2);
  GPIOC.AFR()->setAlternateFunction(9, GPIO::AFR::AlternateFunction::AF2);
}

void initTimer() {
  /* Let's set the prescaler to 1. Increasing the prescaler would slow down the
   * modulation, which can be useful when debugging. */
  TIM3.PSC()->set(1);

  /* Pulse width modulation mode allows you to generate a signal with a
   * frequency determined by the value of the TIMx_ARR register and a duty cycle
   * determined by the value of the TIMx_CCRx register. */
  TIM3.ARR()->set(PWMPeriod);
  TIM3.CCR1()->set(0);
  TIM3.CCR2()->set(0);
  TIM3.CCR3()->set(0);
  TIM3.CCR4()->set(0);

  // Set Channels 1-4 as outputs, PWM mode 1
  TIM3.CCMR()->setOC1M(TIM::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOC2M(TIM::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOC3M(TIM::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOC4M(TIM::CCMR::OCM::PWM1);

  // Output preload enable for channels 1-4
  TIM3.CCMR()->setOC1PE(true);
  TIM3.CCMR()->setOC2PE(true);
  TIM3.CCMR()->setOC3PE(true);
  TIM3.CCMR()->setOC4PE(true);

  // Auto-reload preload enable
  TIM3.CR1()->setARPE(true);

  // Enable Capture/Compare for channel 1 to 4
  TIM3.CCER()->setCC1E(true);
  TIM3.CCER()->setCC2E(true);
  TIM3.CCER()->setCC3E(true);
  TIM3.CCER()->setCC4E(true);

  TIM3.BDTR()->setMOE(true);

  TIM3.CR1()->setCEN(true);
}

}
}
}
