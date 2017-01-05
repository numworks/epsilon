#ifndef ION_DEVICE_LED_H
#define ION_DEVICE_LED_H

namespace Ion {
namespace LED {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PB0 | LED blue          | Alternate Function 2  | TIM3_CH3
 *  PB1 | LED green         | Alternate Function 2  | TIM3_CH4
 *  PC7 | LED red           | Alternate Function 2  | TIM3_CH2
 */

void init();
void initGPIO();
void initTimer();
void suspend();
void resume();

constexpr uint16_t PWMPeriod = 40000;

inline uint16_t dutyCycleForUInt8(uint8_t value) {
  return ((((uint32_t)value + 1)*PWMPeriod)>>8);
}

}
}
}

#endif
