#ifndef ION_FRAMEBUFFER_H
#define ION_FRAMEBUFFER_H

#include <stdint.h>

void ion_set_pixel(uint16_t x, uint16_t y, uint8_t color);

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#endif
