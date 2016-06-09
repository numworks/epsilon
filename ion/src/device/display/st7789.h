#ifndef ION_DEVICE_DISPLAY_ST7789_H
#define ION_DEVICE_DISPLAY_ST7789_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

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
void st7789_set_pixel(st7789_t * controller, uint16_t i, uint16_t j, uint16_t color);
//TODO: void st7789_fill_rect(st7789_t * controller, uint16_t x, uint16_t, ...
void st7789_set_display_area(st7789_t * controller, uint16_t x_start, uint16_t x_length, uint16_t y_start, uint16_t y_length);
//void st7789_enable_frame_data_upload(st7789_t * controller);

#endif
