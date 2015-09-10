/* LCD Initialisation code
 *
 * The LCD panel is connected via an SPI interface. The SPI interface is used to
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
 * The entry point is init_display();
 */

#include <ion.h>
#include "platform.h"
#include "registers/registers.h"
#include <ion/drivers/st7586/st7586.h>

void ion_display_on() {
}

void ion_display_off() {
}

static void init_spi_pins();
static void init_spi_port();
static void init_panel();

void init_display() {
  /* This routine is responsible for initializing the LCD panel.
   * Its interface with the outer world is the framebuffer: after execution of
   * this routine, everyone can expect to write to the LCD by writing to the
   * framebuffer. */
  init_spi_pins();
  init_spi_port();

  init_panel();
}

static void init_spi_pins() {
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

static void init_spi_port() {
  // Enable the SPI2 clock (SPI2 lives on the APB1 bus)
  RCC_APB1ENR |= SPI2EN;

  // Configure the SPI port
  SPI_CR1(SPI2) = (SPI_BIDIMODE | SPI_BIDIOE | SPI_MSTR | SPI_DFF_8_BITS | SPI_BR(SPI_BR_DIV_2) | SPI_SSM | SPI_SSI | SPI_SPE);
}

// Panel

static void spi_2_write(char * data, size_t size);
static void gpio_b10_write(bool pin_state);
static void gpio_b12_write(bool pin_state);
static void gpio_b14_write(bool pin_state);

static void init_panel() {
  gpio_b12_write(1); // LCD-RST high
  Platform.display.chip_select_pin_write = gpio_b10_write;
  Platform.display.data_command_pin_write = gpio_b14_write;
  Platform.display.spi_write = spi_2_write;
  st7586_initialize(&(Platform.display));
}

static void spi_2_write(char * data, size_t size) {
  while (SPI_SR(SPI2) & SPI_BSY) {
  }
  for (size_t i=0; i<size; i++) {
    SPI_DR(SPI2) = data[i];
    while (!(SPI_SR(SPI2) & SPI_TXE)) {
    }
  }
  while (SPI_SR(SPI2) & SPI_BSY) {
  }
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
