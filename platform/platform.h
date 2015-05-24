#ifndef PLATFORM_H
#define PLATFORM_H

#include <platform/fx92kbd/fx92kbd.h>
#include <platform/ili9341/ili9341.h>

typedef struct {
  int framebuffer_width;
  int framebuffer_height;
  int framebuffer_bits_per_pixel;
  char * framebuffer_address;
  fx92kbd_t keyboard;
  ili9341_t display;
} platform_t;

extern platform_t Platform;

#endif
