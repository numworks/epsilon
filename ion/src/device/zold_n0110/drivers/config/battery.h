#ifndef ION_DEVICE_N0110_CONFIG_BATTERY_H
#define ION_DEVICE_N0110_CONFIG_BATTERY_H

#include <regs/regs.h>

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PE3 | BAT_CHRG          | Input, pulled up      | Low = charging, high = full
 *  PB1 | VBAT_SNS          | Analog                | ADC1_1
 */

namespace Ion {
namespace Device {
namespace Battery {
namespace Config {

using namespace Regs;

constexpr static GPIOPin ChargingPin = GPIOPin(GPIOE, 3);
constexpr static GPIOPin ADCPin = GPIOPin(GPIOB, 1);
constexpr uint8_t ADCChannel = 9;
constexpr float ADCReferenceVoltage = 2.8f;
constexpr float ADCDividerBridgeRatio = 2.0f;

}
}
}
}

#endif
