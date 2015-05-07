#ifndef STM32F429_GPIO_H
#define STM32F429_GPIO_H 1

typedef enum {
  STM32_GPIO_GROUP_A = 0,
  STM32_GPIO_GROUP_B = 1,
  STM32_GPIO_GROUP_C = 2,
  STM32_GPIO_GROUP_D = 3,
  STM32_GPIO_GROUP_E = 4,
  STM32_GPIO_GROUP_F = 5,
  STM32_GPIO_GROUP_G = 6,
  STM32_GPIO_GROUP_H = 7,
  STM32_GPIO_GROUP_I = 8,
  STM32_GPIO_GROUP_J = 9,
  STM32_GPIO_GROUP_K = 10
} stm32_gpio_group_t;

typedef struct {
  stm32_gpio_group_t group:4;
  unsigned int number:4;
} stm32_pin_t;

#define STM32_PIN(g,i) (stm32_pin_t){.group = STM32_GPIO_GROUP_##g, .number = i}

#endif
