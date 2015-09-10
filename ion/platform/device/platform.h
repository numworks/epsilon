#ifndef ION_STM32F429_PLATFORM_H
#define ION_STM32F429_PLATFORM_H

#include <ion/drivers/ili9341/ili9341.h>
#include <ion/drivers/fx92kbd/fx92kbd.h>

typedef struct {
  ili9341_t display;
  fx92kbd_t keyboard;
} platform_t;

extern platform_t Platform;

#endif
