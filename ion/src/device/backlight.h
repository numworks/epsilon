#ifndef ION_DEVICE_BACKLIGHT_H
#define ION_DEVICE_BACKLIGHT_H

#include <ion/backlight.h>

namespace Ion {
namespace Backlight {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PC6 | Backlight Enable  | Output                |
 */

void init();
void shutdown();
void suspend();
void resume();
void setLevel(uint8_t level);
uint8_t level();

void sendPulses(int n);

}
}
}

#endif
