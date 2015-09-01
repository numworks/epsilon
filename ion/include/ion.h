#ifndef ION_ION_H
#define ION_ION_H

#include <stdint.h>
#include <ion/framebuffer.h>

void ion_init();

void ion_display_on();
void ion_display_off();

void ion_sleep();

char ion_getchar();

#endif
