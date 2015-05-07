#ifndef STM32_REGISTERS_GPIO_H
#define STM32_REGISTERS_GPIO_H 1

#include <platform/stm32f429/gpio.h>

#pragma mark - GPIO port mode registers

typedef enum {
  GPIO_MODE_INPUT = 0,
  GPIO_MODE_OUTPUT = 1,
  GPIO_MODE_ALTERNATE_FUNCTION = 2,
  GPIO_MODE_ANALOG = 3
} GPIO_MODE_t;

typedef struct {
  GPIO_MODE_t MODER0:2;
  GPIO_MODE_t MODER1:2;
  GPIO_MODE_t MODER2:2;
  GPIO_MODE_t MODER3:2;
  GPIO_MODE_t MODER4:2;
  GPIO_MODE_t MODER5:2;
  GPIO_MODE_t MODER6:2;
  GPIO_MODE_t MODER7:2;
  GPIO_MODE_t MODER8:2;
  GPIO_MODE_t MODER9:2;
  GPIO_MODE_t MODER10:2;
  GPIO_MODE_t MODER11:2;
  GPIO_MODE_t MODER12:2;
  GPIO_MODE_t MODER13:2;
  GPIO_MODE_t MODER14:2;
  GPIO_MODE_t MODER15:2;
} GPIO_MODER_t;

GPIO_MODER_t * GPIO_MODER(stm32_gpio_group_t gpio_group);

#pragma mark - GPIO port output type registers

typedef enum {
  GPIO_OTYPE_PUSH_PULL = 0, // Reset state
  GPIO_OTYPE_OPEN_DRAIN = 1
} GPIO_OTYPE_t;

typedef struct {
  GPIO_OTYPE_t OT0:1;
  GPIO_OTYPE_t OT1:1;
  GPIO_OTYPE_t OT2:1;
  GPIO_OTYPE_t OT3:1;
  GPIO_OTYPE_t OT4:1;
  GPIO_OTYPE_t OT5:1;
  GPIO_OTYPE_t OT6:1;
  GPIO_OTYPE_t OT7:1;
  GPIO_OTYPE_t OT8:1;
  GPIO_OTYPE_t OT9:1;
  GPIO_OTYPE_t OT10:1;
  GPIO_OTYPE_t OT11:1;
  GPIO_OTYPE_t OT12:1;
  GPIO_OTYPE_t OT13:1;
  GPIO_OTYPE_t OT14:1;
  GPIO_OTYPE_t OT15:1;
  unsigned int :16;
} GPIO_OTYPER_t;

GPIO_OTYPER_t * GPIO_OTYPER(stm32_gpio_group_t gpio_group);

#pragma mark - GPIO port output speed registers

typedef enum {
  GPIO_OSPEED_LOW_SPEED = 0, // Reset state, execpt parts of GPIOA/B
  GPIO_OSPEED_MEDIUM_SPEED = 1,
  GPIO_OSPEED_FAST_SPEED = 2,
  GPIO_OSPEED_HIGH_SPEED = 3
} GPIO_OSPEED_t;

typedef struct {
  GPIO_OSPEED_t OSPEEDR0:2;
  GPIO_OSPEED_t OSPEEDR1:2;
  GPIO_OSPEED_t OSPEEDR2:2;
  GPIO_OSPEED_t OSPEEDR3:2;
  GPIO_OSPEED_t OSPEEDR4:2;
  GPIO_OSPEED_t OSPEEDR5:2;
  GPIO_OSPEED_t OSPEEDR6:2;
  GPIO_OSPEED_t OSPEEDR7:2;
  GPIO_OSPEED_t OSPEEDR8:2;
  GPIO_OSPEED_t OSPEEDR9:2;
  GPIO_OSPEED_t OSPEEDR10:2;
  GPIO_OSPEED_t OSPEEDR11:2;
  GPIO_OSPEED_t OSPEEDR12:2;
  GPIO_OSPEED_t OSPEEDR13:2;
  GPIO_OSPEED_t OSPEEDR14:2;
  GPIO_OSPEED_t OSPEEDR15:2;
} GPIO_OSPEEDR_t;

GPIO_OSPEEDR_t * GPIO_OSPEEDR(stm32_gpio_group_t gpio_group);

#pragma mark - GPIO port pull-up/pull-down registers

typedef enum {
  GPIO_PUPD_NONE = 0, // Reset state, except parts of GPIOA/B
  GPIO_PUPD_PULL_UP = 1,
  GPIO_PUPD_PULL_DOWN = 2
} GPIO_PUPD_t;

typedef struct {
  GPIO_PUPD_t PUPDR0:2;
  GPIO_PUPD_t PUPDR1:2;
  GPIO_PUPD_t PUPDR2:2;
  GPIO_PUPD_t PUPDR3:2;
  GPIO_PUPD_t PUPDR4:2;
  GPIO_PUPD_t PUPDR5:2;
  GPIO_PUPD_t PUPDR6:2;
  GPIO_PUPD_t PUPDR7:2;
  GPIO_PUPD_t PUPDR8:2;
  GPIO_PUPD_t PUPDR9:2;
  GPIO_PUPD_t PUPDR10:2;
  GPIO_PUPD_t PUPDR11:2;
  GPIO_PUPD_t PUPDR12:2;
  GPIO_PUPD_t PUPDR13:2;
  GPIO_PUPD_t PUPDR14:2;
  GPIO_PUPD_t PUPDR15:2;
} GPIO_PUPDR_t;

GPIO_PUPDR_t * GPIO_PUPDR(stm32_gpio_group_t gpio_group);

#pragma mark - GPIO port input data registers

typedef struct {
  unsigned int IDR0:1;
  unsigned int IDR1:1;
  unsigned int IDR2:1;
  unsigned int IDR3:1;
  unsigned int IDR4:1;
  unsigned int IDR5:1;
  unsigned int IDR6:1;
  unsigned int IDR7:1;
  unsigned int IDR8:1;
  unsigned int IDR9:1;
  unsigned int IDR10:1;
  unsigned int IDR11:1;
  unsigned int IDR12:1;
  unsigned int IDR13:1;
  unsigned int IDR14:1;
  unsigned int IDR15:1;
  unsigned int :16;
} GPIO_IDR_t;

GPIO_IDR_t * GPIO_IDR(stm32_gpio_group_t gpio_group);

#pragma mark - GPIO port output data registers

typedef struct {
  unsigned int ODR0:1;
  unsigned int ODR1:1;
  unsigned int ODR2:1;
  unsigned int ODR3:1;
  unsigned int ODR4:1;
  unsigned int ODR5:1;
  unsigned int ODR6:1;
  unsigned int ODR7:1;
  unsigned int ODR8:1;
  unsigned int ODR9:1;
  unsigned int ODR10:1;
  unsigned int ODR11:1;
  unsigned int ODR12:1;
  unsigned int ODR13:1;
  unsigned int ODR14:1;
  unsigned int ODR15:1;
  unsigned int :16;
} GPIO_ODR_t;

GPIO_ODR_t * GPIO_ODR(stm32_gpio_group_t gpio_group);

#pragma mark - GPIO port bit set/reset registers

typedef struct {
  unsigned int BS0:1;
  unsigned int BS1:1;
  unsigned int BS2:1;
  unsigned int BS3:1;
  unsigned int BS4:1;
  unsigned int BS5:1;
  unsigned int BS6:1;
  unsigned int BS7:1;
  unsigned int BS8:1;
  unsigned int BS9:1;
  unsigned int BS10:1;
  unsigned int BS11:1;
  unsigned int BS12:1;
  unsigned int BS13:1;
  unsigned int BS14:1;
  unsigned int BS15:1;
  unsigned int BR0:1;
  unsigned int BR1:1;
  unsigned int BR2:1;
  unsigned int BR3:1;
  unsigned int BR4:1;
  unsigned int BR5:1;
  unsigned int BR6:1;
  unsigned int BR7:1;
  unsigned int BR8:1;
  unsigned int BR9:1;
  unsigned int BR10:1;
  unsigned int BR11:1;
  unsigned int BR12:1;
  unsigned int BR13:1;
  unsigned int BR14:1;
  unsigned int BR15:1;
} GPIO_BSRR_t;

GPIO_BSRR_t * GPIO_BSRR(stm32_gpio_group_t gpio_group);

#pragma mark - GPIO port configuration lock registers

typedef enum {
  GPIO_LCK_UNLOCKED = 0,
  GPIO_LCK_LOCKED = 1
} GPIO_LCK_t;

typedef enum {
  GPIO_LCK_KEY_INACTIVE = 0,
  GPIO_LCK_KEY_ACTIVE = 1
} GPIO_LCK_KEY_t;

typedef struct {
  GPIO_LCK_t LCK0:1;
  GPIO_LCK_t LCK1:1;
  GPIO_LCK_t LCK2:1;
  GPIO_LCK_t LCK3:1;
  GPIO_LCK_t LCK4:1;
  GPIO_LCK_t LCK5:1;
  GPIO_LCK_t LCK6:1;
  GPIO_LCK_t LCK7:1;
  GPIO_LCK_t LCK8:1;
  GPIO_LCK_t LCK9:1;
  GPIO_LCK_t LCK10:1;
  GPIO_LCK_t LCK11:1;
  GPIO_LCK_t LCK12:1;
  GPIO_LCK_t LCK13:1;
  GPIO_LCK_t LCK14:1;
  GPIO_LCK_t LCK15:1;
  GPIO_LCK_KEY_t LCKK:1;
} GPIO_LCKR_t;

GPIO_LCKR_t * GPIO_LCKR(stm32_gpio_group_t gpio_group);

#pragma mark - GPIO port alternate function registers

typedef enum {
  GPIO_AF_AF0 = 0,
  GPIO_AF_AF1 = 1,
  GPIO_AF_AF2 = 2,
  GPIO_AF_AF3 = 3,
  GPIO_AF_AF4 = 4,
  GPIO_AF_AF5 = 5,
  GPIO_AF_AF6 = 6,
  GPIO_AF_AF7 = 7,
  GPIO_AF_AF8 = 8,
  GPIO_AF_AF9 = 9,
  GPIO_AF_AF10 = 10,
  GPIO_AF_AF11 = 11,
  GPIO_AF_AF12 = 12,
  GPIO_AF_AF13 = 13,
  GPIO_AF_AF14 = 14,
  GPIO_AF_AF15 = 15
} GPIO_AF_t;

typedef struct {
  GPIO_AF_t AFRL0:4;
  GPIO_AF_t AFRL1:4;
  GPIO_AF_t AFRL2:4;
  GPIO_AF_t AFRL3:4;
  GPIO_AF_t AFRL4:4;
  GPIO_AF_t AFRL5:4;
  GPIO_AF_t AFRL6:4;
  GPIO_AF_t AFRL7:4;
} GPIO_AFRL_t;

typedef struct {
  GPIO_AF_t AFRH8:4;
  GPIO_AF_t AFRH9:4;
  GPIO_AF_t AFRH10:4;
  GPIO_AF_t AFRH11:4;
  GPIO_AF_t AFRH12:4;
  GPIO_AF_t AFRH13:4;
  GPIO_AF_t AFRH14:4;
  GPIO_AF_t AFRH15:4;
} GPIO_AFRH_t;

GPIO_AFRL_t * GPIO_AFRL(stm32_gpio_group_t gpio_group);
GPIO_AFRH_t * GPIO_AFRH(stm32_gpio_group_t gpio_group);

#endif
