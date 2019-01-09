#ifndef ION_DEVICE_BATTERY_H
#define ION_DEVICE_BATTERY_H

#include "regs/regs.h"

namespace Ion {
namespace Battery {
namespace Device {

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA0 | BAT_CHRG          | Input, pulled up      | Low = charging, high = full
 *  PA1 | VBAT_SNS          | Analog                | ADC1_1
 */

void init();
void shutdown();
void initGPIO();
void initADC();

constexpr GPIO ChargingGPIO = GPIOA;
constexpr uint8_t ChargingPin = 0;

constexpr GPIO ADCGPIO = GPIOA;
constexpr uint8_t ADCPin = 1;
constexpr uint8_t ADCChannel = 1;

constexpr float ADCReferenceVoltage = 2.8f;
constexpr float ADCDividerBridgeRatio = 2.0f;

}
}
}

#endif
