#ifndef ION_DEVICE_BATTERY_H
#define ION_DEVICE_BATTERY_H

#include "regs/regs.h"

namespace Ion {
namespace Battery {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA0 | BAT SNS           | Analog                | ADC1
 *  PC2 | BAT CHRG          | Input, pulled up      |
 */

void init();

constexpr GPIO ChargingGPIO = GPIOC;
constexpr uint8_t ChargingPin = 2;

}
}
}

#endif
