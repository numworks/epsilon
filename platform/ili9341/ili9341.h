#ifndef PLATFORM_ILI9341_H
#define PLATFORM_ILI9341_H 1

#include <stddef.h>
#include <stdbool.h>

/* This is the ILI9341 driver
 *
 * It just needs to be pointed to a "write" function
 */
typedef struct {
  void (*chip_select_pin_write)(bool pin_state);
  void (*data_command_pin_write)(bool pin_state);
  void (*spi_write)(char * data, size_t size);
} ili9341_t;

void ili9341_initialize(ili9341_t * controller, bool enable_rgb_interface);
void ili9341_set_gamma(ili9341_t * controller);
#endif
