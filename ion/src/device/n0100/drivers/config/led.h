#ifndef ION_DEVICE_N0100_CONFIG_LED_H
#define ION_DEVICE_N0100_CONFIG_LED_H

#include <regs/regs.h>

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PB0 | LED blue          | Alternate Function 2  | TIM3_CH3
 *  PB1 | LED green         | Alternate Function 2  | TIM3_CH4
 *  PC7 | LED red           | Alternate Function 2  | TIM3_CH2
 */

namespace Ion {
namespace Device {
namespace LED {
namespace Config {

constexpr static GPIOPin RGBPins[] = {
  GPIOPin(GPIOC, 7), GPIOPin(GPIOB, 1), GPIOPin(GPIOB, 0)
};

}
}
}
}

#endif
