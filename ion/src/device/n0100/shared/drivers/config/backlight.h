#ifndef ION_DEVICE_N0100_KERNEL_DRIVERS_CONFIG_BACKLIGHT_H
#define ION_DEVICE_N0100_KERNEL_DRIVERS_CONFIG_BACKLIGHT_H

#include <regs/regs.h>

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PC6 | Backlight Enable  | Output                |
 */

namespace Ion {
namespace Device {
namespace Backlight {
namespace Config {

using namespace Regs;

constexpr static GPIOPin BacklightPin = GPIOPin(GPIOC, 6);

}
}
}
}

#endif
