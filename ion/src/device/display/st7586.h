#ifndef ION_DEVICE_DISPLAY_ST7586_H
#define ION_DEVICE_DISPLAY_ST7586_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* This is the ST7586 driver
 *
 * It just needs to be pointed to a "write" function
 */
typedef struct {
  void (*chip_select_pin_write)(bool pin_state);
  void (*data_command_pin_write)(bool pin_state);
  void (*spi_write)(char * data, size_t size);
} st7586_t;

void st7586_initialize(st7586_t * controller);
void st7586_set_display_area(st7586_t * controller, uint16_t x_start, uint16_t x_length, uint16_t y_start, uint16_t y_length);
void st7586_enable_frame_data_upload(st7586_t * controller);

#endif
