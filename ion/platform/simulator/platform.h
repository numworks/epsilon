#ifndef ION_SIMULATOR_PLATFORM_H
#define ION_SIMULATOR_PLATFORM_H

#include <ion/drivers/fltklcd/fltklcd.h>
extern "C" {
#include "framebuffer.h"
}

typedef struct {
  FltkLCD * display;
} platform_t;

extern platform_t Platform;

#endif
