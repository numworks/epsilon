#ifndef ION_STM32F429_FRAMEBUFFER_H
#define ION_STM32F429_FRAMEBUFFER_H

extern char _framebuffer_start;

#define ION_FRAMEBUFFER_ADDRESS ((void *)(&_framebuffer_start))
#define ION_FRAMEBUFFER_WIDTH 240
#define ION_FRAMEBUFFER_HEIGHT 160
#define ION_FRAMEBUFFER_BITS_PER_PIXEL 2

#endif
