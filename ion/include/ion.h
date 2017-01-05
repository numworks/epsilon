#ifndef ION_ION_H
#define ION_ION_H

#include <ion/backlight.h>
#include <ion/battery.h>
#include <ion/display.h>
#include <ion/events.h>
#include <ion/keyboard.h>
#include <ion/led.h>
#include <ion/power.h>


/* ION is not your regular library. It is a library you link against, but it
 * will take care of configuring the whole environment for you. In POSIX terms,
 * ION will implement the "main" function.
 * Don't worry though, once all its initialization will be performed, ION will
 * jump to your code at ion_app, which you have to implement yourself. */

void ion_app();

namespace Ion {

void msleep(long ms);
void usleep(long us);

/* CAUTION: This is a complete reset! */
void reset();

}

#endif
