#include "pinmux.h"

void stm32_configure_pin(stm32_pin_t pin, stm32_pin_function_t function) {
//  GPIOE = pin.group;
//  high_or_low = pin.number > 8 ? high : low;

//  alt_fun_number = AlternateFunctions[pin][function];
  // Step 1 -> Figure the Alternate Function (1,2,3,4)
  // Step 2 -> Grab the mode register
//  GPIO_AFRH(GPIOE)->AFRH12 = GPIO_AF_AF5;
}

