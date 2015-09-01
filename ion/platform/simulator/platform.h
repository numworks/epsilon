#ifndef ION_SIMULATOR_PLATFORM_H
#define ION_SIMULATOR_PLATFORM_H

#include <ion/drivers/fltklcd/fltklcd.h>
#include <ion/drivers/fltkkbd/fltkkbd.h>
extern "C" {
#include "framebuffer.h"
}

typedef struct {
  FltkLCD * display;
  FltkKBD * keyboard;
} platform_t;

extern platform_t Platform;

#endif
