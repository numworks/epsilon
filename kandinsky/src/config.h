#ifndef KANDINSKY_CONFIG_H
#define KANDINSKY_CONFIG_H

#ifdef KD_CONFIG_H
#include "kandinsky_config.h"
#endif

#ifndef KD_FRAMEBUFFER_ADDRESS
#error Kandinsky expects KD_FRAMEBUFFER_ADDRESS to be defined to the start of the framebuffer
#endif

#ifndef KD_FRAMEBUFFER_WIDTH
#error Kandinsky expects KD_FRAMEBUFFER_WIDTH to be defined to the width of the framebuffer
#endif

// FIXME: #define this
typedef uint8_t KDColor;

/*
#include <platform/platform.h>

#define KD_FRAMEBUFFER_ADDRESS Platform.framebuffer_address
#define KD_FRAMEBUFFER_WIDTH Platform.framebuffer_width
typedef uint8_t KDColor;

*/

#endif
