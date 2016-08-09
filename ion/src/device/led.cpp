extern "C" {
#include <ion/led.h>
}
#include "register.h"

extern "C" {
  void ion_led_init();
}

/* Pin used :
 * PA8 -  Red LED - TIM1_CH1
 * PA9 -  Green LED - TIM1_CH2
 * PA10 - Blue LED  -TIM1_CH3
 */

void sleep() {
  for (volatile int i=0;i<100000; i++) {
  }
}

void ion_led_gpio_init() {
  RCC_AHB1ENR()->setGPIOAEN(true);

  /* RED_LED(PA8), GREEN_LED(PA9), BLUE_LED(PA10) are driven using a timer,
   * which is an alternate function. */
  GPIOA.MODER()->setMODER(8, GPIO::MODER::MODE::AlternateFunction);
  GPIOA.MODER()->setMODER(9, GPIO::MODER::MODE::AlternateFunction);

  /* More precisely, we will use AF01, which maps PA8 to TIM1_CH1, PA9 to
   * TIM1_CH2 and PA10 to TIM1_CH3. */
  GPIOA.AFR()->setAFR(8, GPIO::AFR::AlternateFunction::AF1);
  GPIOA.AFR()->setAFR(9, GPIO::AFR::AlternateFunction::AF1);
}

void ion_led_timer_init() {
  /* TIM1 lives on the APB2 bus. Let's enable its clock. */
  RCC_APB2ENR()->setTIM1EN(true);
  TIM1.PSC()->set(1000);

  /* Pulse width modulation mode allows you to generate a signal with a
   * frequency determined by the value of the TIMx_ARR register and a duty cycle
   * determined by the value of the TIMx_CCRx register. */
  TIM1.ARR()->set(40000);
  TIM1.CCR1()->set(20000);
  TIM1.CCR2()->set(20000);


  // Set Channel 1 as output, PWM mode 1
  TIM1.CCMR1()->setOC1M(TIM::CCMR1::OC1M::PWM1);
  TIM1.CCMR1()->setOC2M(TIM::CCMR1::OC2M::PWM2);
  // Set Channel 2 as output, PWM mode 2
//  REGISTER_SET_VALUE(TIM_CCMR1(TIM1), TIM_OC2M, 6);

  // Output preload enable for channel 1 and 2
  TIM1.CCMR1()->setOC1PE(true);
  TIM1.CCMR1()->setOC2PE(true);
  //FIXME: do OC2PE too

  // Auto-reload preload enable
  TIM1.CR1()->setARPE(true);

  // Enable Capture/Compare channel 1 and channel 2
  TIM1.CCER()->setCC1E(true);
  TIM1.CCER()->setCC2E(true);

  TIM1.BDTR()->setMOE(true);

  TIM1.CR1()->setCEN(true);
}

void ion_led_init() {
  ion_led_gpio_init();
  ion_led_timer_init();
}


void ion_led_set_color(ion_color_t color) {
}
