#ifndef ION_DEVICE_BATTERY_H
#define ION_DEVICE_BATTERY_H

#include "regs/regs.h"

namespace Ion {
namespace Battery {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA0 | BAT_SNS           | Analog                | ADC1_0
 *  PC2 | BAT_CHRG          | Input, pulled up      |
 */

void init();
void initGPIO();
void initADC();

constexpr GPIO ChargingGPIO = GPIOC;
constexpr uint8_t ChargingPin = 2;

constexpr GPIO ADCGPIO = GPIOA;
constexpr uint8_t ADCPin = 0;

constexpr float ADCReferenceVoltage = 2.8f;
constexpr float ADCDividerBridgeRatio = 2.0f;

}
}
}

#endif
