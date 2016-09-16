#ifndef ION_DEVICE_LED_H
#define ION_DEVICE_LED_H

namespace Ion {
namespace LED {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA6 | LED red           | Alternate Function 12 | TIM3_CH1
 *  PA7 | LED green         | Alternate Function 12 | TIM3_CH2
 *  PB0 | LED blue          | Alternate Function 12 | TIM3_CH3
 *  PC9 | LCD backlight     | Alternate Function 12 | TIM3_CH4
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
