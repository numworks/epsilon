#ifndef ION_ION_H
#define ION_ION_H

#include <ion/framebuffer.h>
#include <ion/keyboard.h>

/* ION is not your regular library. It is a library you link against, but it
 * will take care of configuring the whole environment for you. In POSIX terms,
 * ION will implement the "main" function.
 * Don't worry though, once all its initialization will be performed, ION will
 * jump to your code at ion_app, which you have to implement yourself. */

void ion_app();

void ion_display_on();
void ion_display_off();

void ion_sleep();

#endif
