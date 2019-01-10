#ifndef ION_DEVICE_BACKLIGHT_H
#define ION_DEVICE_BACKLIGHT_H

#include <ion/backlight.h>
#include "regs/regs.h"

namespace Ion {
namespace Backlight {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PE0 | Backlight Enable  | Output                |
 */

void init();
void shutdown();
void suspend();
void resume();
void setLevel(uint8_t level);
uint8_t level();

void sendPulses(int n);

constexpr static GPIOPin BacklightPin = GPIOPin(GPIOE, 0);

}
}
}

#endif
