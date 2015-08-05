#ifndef ION_FRAMEBUFFER_H
#define ION_FRAMEBUFFER_H

/* This header doesn't do anything per-se. It only forward-includes the proper
 * framebuffer.h file based on the PLATFORM variable, and checks that everything
 * ION needs has been properly defined. */

#ifndef PLATFORM
#error ION expects PLATFORM to be defined
#endif

#define QUOTE(x) QUOTE_1(x)
#define QUOTE_1(x) #x
#define FRAMEBUFFER_HEADER(x) QUOTE(../platform/x/framebuffer.h)
#include FRAMEBUFFER_HEADER(PLATFORM)

#ifndef ION_FRAMEBUFFER_ADDRESS
#error PLATFORM/framebuffer.h should have defined ION_FRAMEBUFFER_ADDRESS
#endif

#ifndef ION_FRAMEBUFFER_WIDTH
#error PLATFORM/framebuffer.h should have defined ION_FRAMEBUFFER_WIDTH
#endif

#ifndef ION_FRAMEBUFFER_HEIGHT
#error PLATFORM/framebuffer.h should have defined ION_FRAMEBUFFER_HEIGHT
#endif

#ifndef ION_FRAMEBUFFER_BITS_PER_PIXEL
#error PLATFORM/framebuffer.h should have defined ION_FRAMEBUFFER_BITS_PER_PIXEL
#endif

#endif
