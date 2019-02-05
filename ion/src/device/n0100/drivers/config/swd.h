#ifndef ION_DEVICE_N0100_CONFIG_SWD_H
#define ION_DEVICE_N0100_CONFIG_SWD_H

#include <regs/regs.h>

/*  Pin | Role              | Mode
 * -----+-------------------+---------------------
 * PA13 | SWDIO             | Alternate Function 0
 * PA14 | SWCLK             | Alternate Function 0
 *  PB3 | SWO               | Alternate Function 0
 */

namespace Ion {
namespace Device {
namespace SWD {
namespace Config {

constexpr static GPIOPin Pins[] = {
  GPIOPin(GPIOA, 13), GPIOPin(GPIOA, 14), GPIOPin(GPIOB, 3)
};

}
}
}
}

#endif
