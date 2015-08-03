#include <ion/src/drivers/ili9341/ili9341.h>
#include <ion/src/drivers/fx92kbd/fx92kbd.h>

typedef struct {
  ili9341_t display;
  fx92kbd_t keyboard;
} platform_t;

extern platform_t Platform;
