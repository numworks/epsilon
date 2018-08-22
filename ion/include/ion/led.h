#ifndef ION_LED_H
#define ION_LED_H

#include <kandinsky/color.h>

namespace Ion {
namespace LED {

void setColor(KDColor c);
void setBlinking(float blinkLength, bool red, bool green, bool blue);
void setCharging(bool isPluggedIn, bool isCharging);

/*
 * This is just a flag to see if we should mess with the LEDs on our own.
 * Especially useful for keeping the LED blinking while in exam mode.
 */
bool getLockState();
void setLockState(bool state);

}
}

#endif
