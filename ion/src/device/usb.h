#ifndef ION_DEVICE_USB_H
#define ION_DEVICE_USB_H

#include "regs/regs.h"
#include "ion.h"
#include "usb/calculator.h"

namespace Ion {
namespace USB {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA9 | VBUS              | Input, pulled down//TODO    | Low = unplugged, high = plugged
 * PA11 | USB D-            | Alternate Function 10 |
 * PA12 | USB D+            | Alternate Function 10 |
 */

constexpr static GPIOPin VbusPin = GPIOPin(GPIOA, 9);
constexpr static GPIOPin DmPin = GPIOPin(GPIOA, 11);
constexpr static GPIOPin DpPin = GPIOPin(GPIOA, 12);

void init();
void shutdown();
void initGPIO();
void shutdownGPIO();

Calculator * calculator();

}
}
}

#endif
