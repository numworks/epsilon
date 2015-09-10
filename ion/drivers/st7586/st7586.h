#ifndef PLATFORM_ST7586_H
#define PLATFORM_ST7586_H 1

#include <string.h>
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
#endif
