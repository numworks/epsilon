#ifndef ION_DEVICE_N0110_CONFIG_BACKLIGHT_H
#define ION_DEVICE_N0110_CONFIG_BACKLIGHT_H

#include <regs/regs.h>

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PE0 | Backlight Enable  | Output                |
 */

namespace Ion {
namespace Device {
namespace Backlight {
namespace Config {

using namespace Regs;

constexpr static GPIOPin BacklightPin = GPIOPin(GPIOE, 0);

}
}
}
}

#endif
