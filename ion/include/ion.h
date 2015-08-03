#ifndef ION_ION_H
#define ION_ION_H

#include <stdint.h>

void ion_init();

void ion_display_on();
void ion_display_off();

extern void * ion_framebuffer_address;
extern uint16_t ion_framebuffer_width;
extern uint16_t ion_framebuffer_height;
extern uint8_t ion_framebuffer_bits_per_pixel;

char ion_getchar();

#endif
