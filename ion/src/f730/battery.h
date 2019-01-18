#ifndef ION_DEVICE_BATTERY_H
#define ION_DEVICE_BATTERY_H

#include "regs/regs.h"

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

constexpr GPIO ChargingGPIO = GPIOE;
constexpr uint8_t ChargingPin = 3;

constexpr GPIO ADCGPIO = GPIOB;
constexpr uint8_t ADCPin = 1;
constexpr uint8_t ADCChannel = 9;

constexpr float ADCReferenceVoltage = 2.8f;
constexpr float ADCDividerBridgeRatio = 2.0f;

}
}
}

#endif
