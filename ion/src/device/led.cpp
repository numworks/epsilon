#include <ion/led.h>
#include <ion/display.h>
#include "led.h"
#include "regs/regs.h"

// Public Ion::LED methods

void Ion::LED::setColor(KDColor c) {
  TIM3.CCR2()->set(Device::dutyCycleForUInt8(c.red()));
  TIM3.CCR3()->set(Device::dutyCycleForUInt8(c.blue()));
  TIM3.CCR4()->set(Device::dutyCycleForUInt8(c.green()));
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
  /* RED_LED(PC7), GREEN_LED(PB1), and BLUE_LED(PB0) are driven using a timer,
   * which is an alternate function. */
  GPIOB.MODER()->setMode(0, GPIO::MODER::Mode::AlternateFunction);
  GPIOB.MODER()->setMode(1, GPIO::MODER::Mode::AlternateFunction);
  GPIOC.MODER()->setMode(7, GPIO::MODER::Mode::AlternateFunction);

  /* More precisely, we will use AF2, which maps PB0 to TIM3_CH2, PB1 to
   * TIM3_CH4, and PC7 to TIM3_CH2. */
  GPIOB.AFR()->setAlternateFunction(0, GPIO::AFR::AlternateFunction::AF2);
  GPIOB.AFR()->setAlternateFunction(1, GPIO::AFR::AlternateFunction::AF2);
  GPIOC.AFR()->setAlternateFunction(7, GPIO::AFR::AlternateFunction::AF2);
}

void initTimer() {
  /* Let's set the prescaler to 1. Increasing the prescaler would slow down the
   * modulation, which can be useful when debugging. */
  TIM3.PSC()->set(1);

  /* Pulse width modulation mode allows you to generate a signal with a
   * frequency determined by the value of the TIMx_ARR register and a duty cycle
   * determined by the value of the TIMx_CCRx register. */
  TIM3.ARR()->set(PWMPeriod);
  TIM3.CCR2()->set(0);
  TIM3.CCR3()->set(0);
  TIM3.CCR4()->set(0);

  // Set Channels 2-4 as outputs, PWM mode 1
  TIM3.CCMR()->setOC2M(TIM::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOC3M(TIM::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOC4M(TIM::CCMR::OCM::PWM1);

  // Output preload enable for channels 2-4
  TIM3.CCMR()->setOC2PE(true);
  TIM3.CCMR()->setOC3PE(true);
  TIM3.CCMR()->setOC4PE(true);

  // Auto-reload preload enable
  TIM3.CR1()->setARPE(true);

  // Enable Capture/Compare for channel 2 to 4
  TIM3.CCER()->setCC2E(true);
  TIM3.CCER()->setCC3E(true);
  TIM3.CCER()->setCC4E(true);

  TIM3.BDTR()->setMOE(true);

  TIM3.CR1()->setCEN(true);
}

void suspend() {
  TIM3.CCMR()->setOC2M(TIM::CCMR::OCM::ForceInactive);
  TIM3.CCMR()->setOC3M(TIM::CCMR::OCM::ForceInactive);
  TIM3.CCMR()->setOC4M(TIM::CCMR::OCM::ForceInactive);
}

void resume() {
  TIM3.CCMR()->setOC2M(TIM::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOC3M(TIM::CCMR::OCM::PWM1);
  TIM3.CCMR()->setOC4M(TIM::CCMR::OCM::PWM1);
}

}
}
}
