#ifndef STM32F429_PINMUX_H
#define STM32F429_PINMUX_H 1

#include "gpio.h"

/* How to use ?
 *
 * configure_pin(PB11, TIM2_CH4);
 * That's it!
 * Who uses this?
 * platform/spi for example:
 * spi_init(SPI2)
 *   -> NO. BAD IDEA. PINMUX has nothing to do with SPI per-se. */

/* Note that the table is pretty large
 * Not using it in production might be an idea.
 * Just don't use "stm32_configure_pin", but address registers manually
 * and the linker will do its job. */

typedef enum {
  STM32_BUS_SYS,
  STM32_BUS_TIM1,
  STM32_BUS_TIM2,
  STM32_BUS_TIM3,
  STM32_BUS_TIM4,
  STM32_BUS_TIM5,
  STM32_BUS_TIM8,
  STM32_BUS_TIM9,
  STM32_BUS_TIM10,
  STM32_BUS_TIM11,
  STM32_BUS_TIM12,
  STM32_BUS_TIM13,
  STM32_BUS_SPI1,
  STM32_BUS_SPI2,
  STM32_BUS_SPI3
} stm32_bus_t;

typedef enum {
  SPI_WIRE_NSS = 0,
  SPI_WIRE_SCK = 1,
  SPI_WIRE_MISO = 2,
  SPI_WIRE_MOSI = 3,

  USART_WIRE_CK = 0,
  USART_WIRE_RX = 1,
  USART_WIRE_TX = 2,
  USART_WIRE_CTS = 3,
  USART_WIRE_RTS = 4
} stm32_wire_t;

typedef struct {
  stm32_bus_t bus:4; // SPI2
  stm32_wire_t wire:4; // MISO
} stm32_pin_function_t;


#define STM32_PIN_FUNCTION(b,i,r) (stm32_pin_function_t){.bus = STM32_BUS_##b##i, .wire = b##_WIRE_##r}
#define P(b,i,r) STM32_PIN_FUNCTION(b,i,r)

stm32_pin_function_t AlternateFunctionMapping[11*16][16] = {
  // This comes from the STM32F429 datasheet
  // Page 73 to 83
  //{X, P(SPI2,MISO), X, X, X},
  //{{}, {}, {.bus = SPI2, .role = MOSI}},
  {{}, P(SPI,2,MISO),{}},
  {},
};

#undef P

// Sample call:
// stm32_configure_pin(STM32_PIN(A,10), STM32_PIN_FUNCTION(SPI,2,MISO));

void stm32_configure_pin(stm32_pin_t pin, stm32_pin_function_t function);

#endif
