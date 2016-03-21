#ifndef ION_DEVICE_DISPLAY_FRAMEBUFFER_H_
#define ION_DEVICE_DISPLAY_FRAMEBUFFER_H_

#include <ion.h>

extern char _framebuffer_start;
extern char _framebuffer_end;

#define FRAMEBUFFER_ADDRESS (&_framebuffer_start)
#define FRAMEBUFFER_LENGTH (&_framebuffer_end-&_framebuffer_start)
#define FRAMEBUFFER_WIDTH SCREEN_WIDTH
#define FRAMEBUFFER_HEIGHT SCREEN_HEIGHT

#define FRAMEBUFFER_BITS_PER_PIXEL 4

#endif // ION_DEVICE_DISPLAY_FRAMEBUFFER_H_
