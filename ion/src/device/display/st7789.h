#ifndef ION_DEVICE_DISPLAY_ST7789_H
#define ION_DEVICE_DISPLAY_ST7789_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ion/framebuffer.h>

#define ST7789_USE_9BIT_SPI 1

typedef struct {
  void (*reset_pin_write)(bool pin_state);
  void (*chip_select_pin_write)(bool pin_state);
  void (*spi_write)(char * data, size_t size);
#if ST7789_USE_9BIT_SPI
  uint32_t spi_queue;
  uint8_t spi_queue_usage_in_bits;
#else
  void (*data_command_pin_write)(bool pin_state);
#endif
} st7789_t;

void st7789_initialize(st7789_t * controller);

void st7789_set_drawing_area(st7789_t * controller,
    int16_t x, int16_t y,
    int16_t width, int16_t height);

void st7789_push_pixels(st7789_t * controller,
    ion_color_t * pixels, int32_t numberOfPixels);
#endif
