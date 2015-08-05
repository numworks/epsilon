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

#ifndef KD_BITS_PER_PIXEL
#error Kandinsky expects KD_BITS_PER_PIXEL to be defined to the number of bits per pixels.
#endif

#endif
