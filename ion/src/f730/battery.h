#ifndef ION_DEVICE_BATTERY_H
#define ION_DEVICE_BATTERY_H

#include "regs/gpio.h"

namespace Ion {
namespace Battery {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PE3 | BAT_CHRG          | Input, pulled up      | Low = charging, high = full
 *  PB1 | VBAT_SNS          | Analog                | ADC1_IN9
 */

void init();
void shutdown();
void initGPIO();
void initADC();

constexpr GPIOPin ChargingPin = GPIOPin(GPIOE, 3);

constexpr GPIOPin VoltagePin = GPIOPin(GPIOB, 1);
constexpr uint8_t ADCChannel = 9;

constexpr float ADCReferenceVoltage = 2.8f;
constexpr float ADCDividerBridgeRatio = 2.0f;

}
}
}

#endif
