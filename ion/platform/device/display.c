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
#include <string.h>

#include "display/gpio.h"
#include "display/spi.h"
#include "display/dma.h"

#include "platform.h"
#include "framebuffer.h"
#include "registers/registers.h"
#include <ion/drivers/st7586/st7586.h>
#include "display/dma.h"

void ion_display_on() {
  // Initialize panel
  // Start DMA transfer
}

void ion_display_off() {
  // Stop DMA transfer
  // Turn off panel
}

void init_display() {
  display_gpio_init();
  display_spi_init();

  st7586_t * controller = &(Platform.display);
  gpio_b12_write(1); // LCD-RST high
  controller->chip_select_pin_write = gpio_b10_write;
  controller->data_command_pin_write = gpio_b14_write;
  controller->spi_write = spi_2_write;

  st7586_initialize(controller);

  st7586_set_display_area(controller, 0, FRAMEBUFFER_WIDTH, 0, FRAMEBUFFER_HEIGHT);

  st7586_enable_frame_data_upload(controller);

  memset(FRAMEBUFFER_ADDRESS, 0, FRAMEBUFFER_LENGTH);

  for (int i=0;i<10;i++) {
    ion_set_pixel(i,i,0x3);
  }

  display_dma_init();
}
