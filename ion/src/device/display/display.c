/* LCD Initialisation code
 *
 * The LCD panel is connected via an SPI interface. The SPI interface is used to
 * configure the panel and to stream the bitmap data.
 *
 * Here is the connection this file assumes:
 *
 *  LCD          | STM32 | Role
 * --------------+-------+-----
 *  LCD_CS       | PC13  | Chip-select for LCD panel (panel selected when low)
 *  LCD_RST      | PB12  | Reset the LCD panel (panel active when high)
 *  LCD_DAT_INS  | PB14  | Select "command" or "data" mode
 *  LCD_SPI_CLK  | PB13  | SPI clock
 *  LCD_SPI_MOSI | PB15  | SPI data
 *
 * The entry point is init_display();
 */

#include <ion.h>
#include <string.h>
#include <assert.h>

#include "../registers/registers.h"

#include "gpio.h"
#include "spi.h"
#include "dma.h"
#include "framebuffer.h"
#include "st7789.h"

static st7789_t sDisplayController;

void ion_display_on() {
  // Initialize panel
  // Start DMA transfer
}

void ion_display_off() {
  // Stop DMA transfer
  // Turn off panel
}

void ion_screen_push_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const ion_color_t * pixels) {
  st7789_set_drawing_area(&sDisplayController, x, y, width, height);
  st7789_push_pixels(&sDisplayController, pixels, width*height);
}

void ion_screen_push_rect_uniform(uint16_t x, uint16_t y, uint16_t width, uint16_t height, ion_color_t color) {
  st7789_set_drawing_area(&sDisplayController, x, y, width, height);
  for (size_t i=0; i<width*height; i++) {
    st7789_push_pixels(&sDisplayController, &color, 1);
  }
}

void ion_screen_pull_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, ion_color_t * pixels) {
  assert(0); // Unimplemented
}

#if 0
void ion_screen_push_rect(
    uint16_t x, uint16_t y,
    uint16_t width, uint16_t height,
    ion_color_t * pattern, uint16_t patternWidth, uint16_t patternHeight)
{
  st7789_set_drawing_area(&sDisplayController, x, y, width, height);
#if ION_DEVICE_FILL_RECT_FAST_PATH
  /* If the pattern width matches the target rect width, we can easily push
   * mutliple lines at once since those will be contiguous in memory. */
  if (patternWidth == width) {
    size_t remainingSize = width*height;
    size_t patternSize = patternWidth*patternHeight;
    while (remainingSize > 0) {
      int32_t blockSize = remainingSize > patternSize ? patternSize : remainingSize;
      st7789_push_pixels(&sDisplayController, pattern, blockSize);
      remainingSize -= blockSize;
    }
    return;
  }
#endif
  uint16_t remainingHeight = height;
  uint16_t patternLine = 0;
  while (remainingHeight-- > 0) {
    uint16_t remainingWidth = width;
    while (remainingWidth > 0) {
      int32_t blockSize = remainingWidth > patternWidth ? patternWidth : remainingWidth;
      st7789_push_pixels(&sDisplayController, pattern+patternLine*patternWidth, blockSize);
      remainingWidth -= blockSize;
    }
    if (++patternLine >= patternHeight) {
      patternLine = 0;
    }
  }
}
#endif

void init_display() {
  //assert(FRAMEBUFFER_LENGTH == (FRAMEBUFFER_WIDTH*FRAMEBUFFER_HEIGHT*FRAMEBUFFER_BITS_PER_PIXEL)/8);

  display_gpio_init();
  display_spi_init();

  sDisplayController.chip_select_pin_write = gpio_c13_write;
  sDisplayController.reset_pin_write = gpio_b12_write;
  //sDisplayController.data_command_pin_write = gpio_b14_write;
  sDisplayController.spi_write = spi_2_write;

  st7789_initialize(&sDisplayController);

  //st7789_set_display_area(&sDisplayController, 0, 240, 0, 320);

  //st7789_enable_frame_data_upload(&sDisplayController);

  /*while (1) {
  }*/

  //memset(FRAMEBUFFER_ADDRESS, 0, FRAMEBUFFER_LENGTH);

  //display_dma_init();
}
