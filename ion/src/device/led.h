#ifndef ION_DEVICE_LED_H
#define ION_DEVICE_LED_H

#include "regs/regs.h"

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
void shutdown();
/* This call bypasses the timer, and immediately enforces a given LED state. */
void enforceState(bool red, bool green, bool blue);

void initGPIO();
void shutdownGPIO();
void initTimer();
void shutdownTimer();

constexpr static GPIOPin RGBPins[] = {
  GPIOPin(GPIOC, 7), GPIOPin(GPIOB, 1), GPIOPin(GPIOB, 0)
};


constexpr uint16_t PWMPeriod = 40000;

inline uint16_t dutyCycleForUInt8(uint8_t value) {
  /* This function is a linear function from colors [0->255] to duty cycles [0->PWMPeriod].*/
  return ((uint32_t)value)*(PWMPeriod/255);
}

}
}
}

#endif
