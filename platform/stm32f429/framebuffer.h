#ifndef STM32F4_FRAMEBUFFER_H
#define STM32F4_FRAMEBUFFER_H

#include <stdint.h>

typedef uint8_t pixel_t;

#define FRAMEBUFFER_WIDTH 240
#define FRAMEBUFFER_HEIGHT 320
#define FRAMEBUFFER_BITS_PER_PIXEL 8
#define FRAMEBUFFER_ADDRESS (pixel_t *)(0x2001D400)

#endif
