#ifndef ION_DEVICE_KERNEL_DRIVERS_BATTERY_H
#define ION_DEVICE_KERNEL_DRIVERS_BATTERY_H

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

bool isCharging();

Ion::Battery::Charge level();

float voltage();


void init();
void shutdown();
void initGPIO();
void initADC();

}
}
}

#endif

