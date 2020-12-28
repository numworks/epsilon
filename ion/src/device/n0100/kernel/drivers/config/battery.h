#ifndef ION_DEVICE_N0100_KERNEL_DRIVERS_CONFIG_BATTERY_H
#define ION_DEVICE_N0100_KERNEL_DRIVERS_CONFIG_BATTERY_H

#include <regs/regs.h>

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA0 | BAT_CHRG          | Input, pulled up      | Low = charging, high = full
 *  PA1 | VBAT_SNS          | Analog                | ADC1_1
 */

namespace Ion {
namespace Device {
namespace Battery {
namespace Config {

using namespace Regs;

constexpr static GPIOPin ChargingPin = GPIOPin(GPIOA, 0);
constexpr static GPIOPin ADCPin = GPIOPin(GPIOA, 1);
constexpr uint8_t ADCChannel = 1;
constexpr float ADCReferenceVoltage = 2.8f;
constexpr float ADCDividerBridgeRatio = 2.0f;

}
}
}
}

#endif
