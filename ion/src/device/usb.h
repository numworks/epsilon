#ifndef ION_DEVICE_USB_H
#define ION_DEVICE_USB_H

#include "regs/regs.h"

namespace Ion {
namespace USB {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA9 | VBUS              | Input, pulled down    | Low = unplugged, high = plugged
 * PA11 | USB D-            | Alternate Function 10 |
 * PA12 | USB D+            | Alternate Function 10 |
 */

void init();
void initGPIO();
void shutdown();

constexpr static GPIOPin VbusPin = GPIOPin(GPIOA, 9);
constexpr static GPIOPin DmPin = GPIOPin(GPIOA, 11);
constexpr static GPIOPin DpPin = GPIOPin(GPIOA, 12);

}
}
}

#endif
