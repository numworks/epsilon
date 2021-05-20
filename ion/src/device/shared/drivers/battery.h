#ifndef ION_DEVICE_SHARED_DRIVERS_BATTERY_H
#define ION_DEVICE_SHARED_DRIVERS_BATTERY_H

#include <stdint.h>
#include <ion/battery.h>

/*
 *
 * RAIN = External input impedance should be <= to
 * (k-0.5)/(fADC * CADC * ln(2^(n+2)) - RADC
 *
 * n = resolution = 12
 * k = number of sampling periods
 */

namespace Ion {
namespace Device {
namespace Battery {

Ion::Battery::Charge level();
float voltage();

void init();
void shutdown();
void initGPIO();

}
}
}

#endif

