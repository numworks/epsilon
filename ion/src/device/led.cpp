#include <ion/led.h>
#include "led.h"
#include "regs/regs.h"

// Public Ion::LED methods

void Ion::LED::setColor(KDColor c) {
  TIM1.CCR1()->set(15000);
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
  /* RED_LED(PA8), GREEN_LED(PA9), BLUE_LED(PA10) are driven using a timer,
   * which is an alternate function. */
  GPIOA.MODER()->setMode(8, GPIO::MODER::Mode::AlternateFunction);
  GPIOA.MODER()->setMode(9, GPIO::MODER::Mode::AlternateFunction);
  GPIOA.MODER()->setMode(10, GPIO::MODER::Mode::AlternateFunction);

  /* More precisely, we will use AF01, which maps PA8 to TIM1_CH1, PA9 to
   * TIM1_CH2 and PA10 to TIM1_CH3. */
  GPIOA.AFR()->setAlternateFunction(8, GPIO::AFR::AlternateFunction::AF1);
  GPIOA.AFR()->setAlternateFunction(9, GPIO::AFR::AlternateFunction::AF1);
  GPIOA.AFR()->setAlternateFunction(10, GPIO::AFR::AlternateFunction::AF1);
}

void initTimer() {
  /* TIM1 lives on the APB2 bus. Let's enable its clock. */
  RCC.APB2ENR()->setTIM1EN(true);
  TIM1.PSC()->set(1000);

  /* Pulse width modulation mode allows you to generate a signal with a
   * frequency determined by the value of the TIMx_ARR register and a duty cycle
   * determined by the value of the TIMx_CCRx register. */
  TIM1.ARR()->set(40000);
  TIM1.CCR1()->set(15000);
  TIM1.CCR2()->set(20000);
  TIM1.CCR3()->set(8000);

  // Set Channel 1 as output, PWM mode 1
  //auto ccmr = *(uint32_t *)TIM1.CCMR();
  //auto ccmr = *(TIM1.CCMR());
  //class TIM::CCMR ccmr(*(TIM1.CCMR()));
  TIM1.CCMR()->setOC1M(TIM::CCMR::OCM::PWM1);
  TIM1.CCMR()->setOC2M(TIM::CCMR::OCM::PWM1);
  TIM1.CCMR()->setOC3M(TIM::CCMR::OCM::PWM1);
  // Set Channel 2 as output, PWM mode 2
//  REGISTER_SET_VALUE(TIM_CCMR1(TIM1), TIM_OC2M, 6);

  // Output preload enable for channel 1 and 2
  TIM1.CCMR()->setOC1PE(true);
  TIM1.CCMR()->setOC2PE(true);
  TIM1.CCMR()->setOC3PE(true);

  // Auto-reload preload enable
  TIM1.CR1()->setARPE(true);

  // Enable Capture/Compare channel 1 and channel 2
  TIM1.CCER()->setCC1E(true);
  TIM1.CCER()->setCC2E(true);
  TIM1.CCER()->setCC3E(true);

  TIM1.BDTR()->setMOE(true);

  TIM1.CR1()->setCEN(true);
}

}
}
}
