#include <registers/gpio.h>

#define GPIOA_BASE 0x40020000
#define GPIOB_BASE 0x40020400
#define GPIOC_BASE 0x40020800
#define GPIOD_BASE 0x40020C00
#define GPIOE_BASE 0x40021000
#define GPIOF_BASE 0x40021400
#define GPIOG_BASE 0x40021800
#define GPIOH_BASE 0x40021C00
#define GPIOI_BASE 0x40022000
#define GPIOJ_BASE 0x40022400
#define GPIOK_BASE 0x40022800

#define GPIO_MODER_OFFSET   0x00
#define GPIO_OTYPER_OFFSET  0x04
#define GPIO_OSPEEDR_OFFSET 0x08
#define GPIO_PUPDR_OFFSET   0x0C
#define GPIO_IDR_OFFSET     0x10

char * GPIO_REGISTER_ADDRESS(GPIO_t gpio, int registerOffset) {
  char * gpioBaseAddress[11] = {
    (char *)GPIOA_BASE, (char *)GPIOB_BASE, (char *)GPIOC_BASE,
    (char *)GPIOD_BASE, (char *)GPIOE_BASE, (char *)GPIOF_BASE,
    (char *)GPIOG_BASE, (char *)GPIOH_BASE, (char *)GPIOI_BASE,
    (char *)GPIOJ_BASE, (char *)GPIOK_BASE
  };
  return gpioBaseAddress[gpio] + registerOffset;
}


GPIO_MODER_t * GPIO_MODER(GPIO_t gpio) {
  return (GPIO_MODER_t *)GPIO_REGISTER_ADDRESS(gpio, GPIO_MODER_OFFSET);
}

GPIO_OTYPER_t * GPIO_OTYPER(GPIO_t gpio) {
  return (GPIO_OTYPER_t *)GPIO_REGISTER_ADDRESS(gpio, GPIO_OTYPER_OFFSET);
}

GPIO_OSPEEDR_t * GPIO_OSPEEDR(GPIO_t gpio) {
  return (GPIO_OSPEEDR_t *)GPIO_REGISTER_ADDRESS(gpio, GPIO_OSPEEDR_OFFSET);
}

GPIO_PUPDR_t * GPIO_PUPDR(GPIO_t gpio) {
  return (GPIO_PUPDR_t *)GPIO_REGISTER_ADDRESS(gpio, GPIO_PUPDR_OFFSET);
}

GPIO_IDR_t * GPIO_IDR(GPIO_t gpio) {
  return (GPIO_IDR_t *)GPIO_REGISTER_ADDRESS(gpio, GPIO_IDR_OFFSET);
}

