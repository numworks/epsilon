#include <ion/led.h>
#include "registers/registers.h"

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
  // We are using GPIOA, which live on the AHB1 bus. Let's enable its clock.
  RCC_AHB1ENR |= GPIOAEN;

#if 0

  for (int i=8; i<=8; i++) {
    REGISTER_SET_VALUE(GPIO_MODER(GPIOA), MODER(i), GPIO_MODE_OUTPUT);
  }
  char value = 1;
  while (1) {
    value = !value;
    for (int i=8; i<=10; i++) {
      REGISTER_SET_VALUE(GPIO_ODR(GPIOA), ODR(i), value);
    }
    for (volatile int i=0; i<100000; i++) {
    }
  }
#else

  /* RED_LED(PA8), GREEN_LED(PA9), BLUE_LED(PA10) are driven using a timer,
   * which is an alternate function. */
  REGISTER_SET_VALUE(GPIO_MODER(GPIOA), MODER(8), GPIO_MODE_ALTERNATE_FUNCTION);
  REGISTER_SET_VALUE(GPIO_MODER(GPIOA), MODER(9), GPIO_MODE_ALTERNATE_FUNCTION);
  REGISTER_SET_VALUE(GPIO_MODER(GPIOA), MODER(10), GPIO_MODE_ALTERNATE_FUNCTION);

  /* More precisely, we will use AF01, which maps PA8 to TIM1_CH1, PA9 to
   * TIM1_CH2 and PA10 to TIM1_CH3. */
  REGISTER_SET_VALUE(GPIO_AFR(GPIOA, 8), AFR(8), 1);
  REGISTER_SET_VALUE(GPIO_AFR(GPIOA, 9), AFR(9), 1);
  REGISTER_SET_VALUE(GPIO_AFR(GPIOA, 10), AFR(10), 1);
#endif
}

void ion_led_timer_init() {
  /* TIM1 lives on the APB2 bus. Let's enable its clock. */
  RCC_APB2ENR |= TIM1EN;
  TIM_PSC(TIM1) = 1000;

  /* Pulse width modulation mode allows you to generate a signal with a
   * frequency determined by the value of the TIMx_ARR register and a duty cycle
   * determined by the value of the TIMx_CCRx register. */
  TIM_ARR(TIM1) = 40000;
  TIM_CCR1(TIM1) = 20000;
  TIM_CCR2(TIM1) = 20000;


  // Set Channel 1 as output, PWM mode 1
  REGISTER_SET_VALUE(TIM_CCMR1(TIM1), TIM_OC1M, 6);
  // Set Channel 2 as output, PWM mode 2
  REGISTER_SET_VALUE(TIM_CCMR1(TIM1), TIM_OC2M, 6);
  //REGISTER_SET_VALUE(TIM_CCMR2(TIM1), TIM_OC3M, 6);

  // Output preload enable for channel 1 and 2
  TIM_CCMR1(TIM1) |= (TIM_OC1PE | TIM_OC2PE);

  // Auto-reload preload enable
  TIM_CR1(TIM1) |= TIM_ARPE;

  // Enable Capture/Compare channel 1 and channel 2
  TIM_CCER(TIM1) |= (TIM_CC1E | TIM_CC2E);

  TIM_BDTR(TIM1) |= (TIM_MOE);

  TIM_CR1(TIM1) |= (TIM_CEN); // Enable the counter
}

void ion_led_init() {
  ion_led_gpio_init();
  ion_led_timer_init();
}


void ion_led_set_color(ion_color_t color) {
}
