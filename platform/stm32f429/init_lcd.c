/* LCD Initialisation code
 *
 * The LCD panel is connected via two interfaces: RGB and SPI. The SPI interface
 * is used to configure the panel, and can be used to send pixel data.
 * For higher performances, the RGB interface can be used to send pixel data.
 *
 * Here is the connection this file assumes:
 *
 *  LCD_SPI | STM32 | Role
 * ---------+-------+-----
 *    RESET | NRST  |
 *      CSX | PC2   | Chip enable input
 *      DCX | PD13  | Selects "command" or data mode
 *      SCL | PF7   | SPI clock
 *  SDI/SDO | PF9   | SPI data
 *
 * The entry point is init_lcd();
 *
 * Some info regarding the built-in LCD panel of the STM32F429I Discovery:
 *  -> The pin EXTC of the ili9341 is not connected to Vdd. It reads as "0", and
 *  therefore extended registers are not available. Those are 0xB0-0xCF and 0xE0
 *  - 0xFF. Apparently this means we cannot read the display ID (RDDIDIF).
 *  That's wat ST says in stm32f429i_discovery_lcd.c.
 */

#include "registers/rcc.h"
#include "registers/gpio.h"
#include "registers/spi.h"
#include <platform/ili9341/ili9341.h>

static void spi_5_write(char * data, size_t size);
static void gpio_c2_write(bool pin_state);
static void gpio_d13_write(bool pin_state);

ili9341_t sPanel = {
  .chip_select_pin_write = gpio_c2_write,
  .data_command_pin_write = gpio_d13_write,
  .spi_write = spi_5_write
};

static void init_lcd_gpio();
static void init_lcd_spi();
static void init_lcd_panel();

void init_lcd() {
  /* This routine is responsible for initializing the LCD panel.
   * Its interface with the outer world is the framebuffer: after execution of
   * this routine, everyone can expect to write to the LCD by writing to the
   * framebuffer. */

  /* The LCD panel is connected on GPIO pins. Let's configure them. */
  init_lcd_gpio();

  /* According to our GPIO config, we send commands to the LCD panel over SPI on
   * port SPI5. Let's configure it. */
  init_lcd_spi();

  /* configure display */
  init_lcd_panel();

  /* Last but not least */
  //TODO: init_lcd_dma();
}

#pragma mark - GPIO initialization

static void init_lcd_gpio_clocks() {
  // We are using groups C, D, and F. Let's enable their clocks
  RCC_AHB1ENR->GPIOCEN = 1;
  RCC_AHB1ENR->GPIODEN = 1;
  RCC_AHB1ENR->GPIOFEN = 1;
}

static void init_lcd_gpio_modes() {
  // PC2 and PD13 are controlled directly
  GPIO_MODER(GPIOC)->MODER2 = GPIO_MODE_OUTPUT;
  GPIO_MODER(GPIOD)->MODER13 = GPIO_MODE_OUTPUT;

  // PF7 and PF9 are used for an alternate function (in that case, SPI)
  GPIO_MODER(GPIOF)->MODER7 = GPIO_MODE_ALTERNATE_FUNCTION;
  GPIO_MODER(GPIOF)->MODER9 = GPIO_MODE_ALTERNATE_FUNCTION;

  // More precisely, PF7 and PF9 are doing SPI-SCL and SPI-SDO/SDO.
  // This corresponds to Alternate Function 5 using SPI port 5
  // (See STM32F429 p78)
  GPIO_AFRL(GPIOF)->AFRL7 = GPIO_AF_AF5; // Pin 7 is in the "low" register
  GPIO_AFRH(GPIOF)->AFRH9 = GPIO_AF_AF5; // and pin 9 in the "high" one

  // For debug
  /*
  GPIO_MODER(GPIOF)->MODER6 = GPIO_MODE_ALTERNATE_FUNCTION;
  GPIO_MODER(GPIOF)->MODER8 = GPIO_MODE_ALTERNATE_FUNCTION;
  GPIO_AFRL(GPIOF)->AFRL6 = GPIO_AF_AF5; // and pin 9 in the "high" one
  GPIO_AFRH(GPIOF)->AFRH8 = GPIO_AF_AF5; // and pin 9 in the "high" one
  */
}

static void init_lcd_gpio() {
  /* The LCD panel is connected on GPIO pins. Let's configure them. */
  init_lcd_gpio_clocks();
  init_lcd_gpio_modes();
}

void gpio_c2_write(bool pin_state) {
  GPIO_ODR(GPIOC)->ODR2 = pin_state;
}

void gpio_d13_write(bool pin_state) {
  GPIO_ODR(GPIOD)->ODR13 = pin_state;
}

#pragma mark - SPI initialization

static void init_lcd_spi() {
  // Enable the SPI5 clock (SPI5 lives on the APB2 bus)
  RCC_APB2ENR->SPI5EN = 1;

  // Configure the SPI port
  // Using a C99 compound litteral. C99 guarantees all non-set values are zero
  *SPI_CR1(SPI5) = (SPI_CR1_t){
    .BIDIMODE = 1,
    .BIDIOE = 1,
    .MSTR = 1,
    .DFF = SPI_DFF_8_BITS,
    .CPOL = 0,
    .CPHA = 0,
    .BR = SPI_BR_DIV_2,
    .SSM = 1,
    .SSI = 1,
    .LSBFIRST = 0, // Send the most significant bit first
    .SPE = 1
  };
}

static void spi_5_write(char * data, size_t size) {
  volatile SPI_SR_t * spi_status = SPI_SR(SPI5);
  volatile SPI_DR_t * spi_data_register = SPI_DR(SPI5);

  while (spi_status->BSY != 0) {
  }
  for (size_t i=0; i<size; i++) {
    *spi_data_register = data[i];
    while (spi_status->TXE == 0) {
    }
  }
  while (spi_status->BSY != 0) {
  }
}

#pragma mark - Panel initialization

void init_lcd_panel() {
  ili9341_initialize(&sPanel);
}
