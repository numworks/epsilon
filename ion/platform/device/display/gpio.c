/* The LCD panel is connected via an SPI interface. The SPI interface is used to
 * configure the panel and to stream the bitmap data.
 *
 * Here is the connection this file assumes:
 *
 *  LCD          | STM32 | Role
 * --------------+-------+-----
 *  LCD_CS       | PB10  | Chip-select for LCD panel (panel selected when low)
 *  LCD_RST      | PB12  | Reset the LCD panel (panel active when high)
 *  LCD_DAT_INS  | PB14  | Select "command" or "data" mode
 *  LCD_SPI_CLK  | PB13  | SPI clock
 *  LCD_SPI_MOSI | PB15  | SPI data
 *
 */

#include "../registers/registers.h"
#include "gpio.h"

void display_gpio_init() {
  // We are using GPIO B, which live on the AHB1 bus. Let's enable its clock.
  RCC_AHB1ENR |= GPIOBEN;

  // LCD_CS(PB10, LCD_RST(PB12) and LCD_DAT_INS(PB14)are controlled directly
  REGISTER_SET_VALUE(GPIO_MODER(GPIOB), MODER(10), GPIO_MODE_OUTPUT);
  REGISTER_SET_VALUE(GPIO_MODER(GPIOB), MODER(12), GPIO_MODE_OUTPUT);
  REGISTER_SET_VALUE(GPIO_MODER(GPIOB), MODER(14), GPIO_MODE_OUTPUT);

  /* LCD_SPI_CLK(PB13) and LCD_SPI_MOSI(PB15) are used for SPI, which is an
   * alternate function. */
  REGISTER_SET_VALUE(GPIO_MODER(GPIOB), MODER(13), GPIO_MODE_ALTERNATE_FUNCTION);
  REGISTER_SET_VALUE(GPIO_MODER(GPIOB), MODER(15), GPIO_MODE_ALTERNATE_FUNCTION);

  /* More precisely, we will use AF05, which maps PB15 to SPI2_MOSI and PB13 to
   * SPI2_SCK. */
  REGISTER_SET_VALUE(GPIO_AFR(GPIOB, 13), AFR(13), 5);
  REGISTER_SET_VALUE(GPIO_AFR(GPIOB, 15), AFR(15), 5);
}

void gpio_b10_write(bool pin_state) {
  REGISTER_SET_VALUE(GPIO_ODR(GPIOB), ODR(10), pin_state);
}

void gpio_b12_write(bool pin_state) {
  REGISTER_SET_VALUE(GPIO_ODR(GPIOB), ODR(12), pin_state);
}

void gpio_b14_write(bool pin_state) {
  REGISTER_SET_VALUE(GPIO_ODR(GPIOB), ODR(14), pin_state);
}
