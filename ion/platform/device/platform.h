#ifndef ION_STM32F429_PLATFORM_H
#define ION_STM32F429_PLATFORM_H

#include <ion/drivers/st7586/st7586.h>
#include <ion/drivers/fx92kbd/fx92kbd.h>

typedef struct {
  st7586_t display;
  fx92kbd_t keyboard;
} platform_t;

extern platform_t Platform;

#endif
