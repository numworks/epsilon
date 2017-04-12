#ifndef ION_DEVICE_USB_H
#define ION_DEVICE_USB_H

#include "regs/regs.h"

namespace Ion {
namespace USB {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA9 | VBUS              | Input, pulled down    | Low = unplugged, high = plugged
 */

void init();
void initGPIO();
void shutdown();

constexpr static GPIOPin VbusPin = GPIOPin(GPIOA, 9);

}
}
}

#endif
