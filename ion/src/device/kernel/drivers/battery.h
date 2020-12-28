#ifndef ION_DEVICE_KERNEL_DRIVERS_BATTERY_H
#define ION_DEVICE_KERNEL_DRIVERS_BATTERY_H

#include <stdint.h>

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

enum class Charge : uint8_t {
  EMPTY = 0,
  LOW = 1,
  SOMEWHERE_INBETWEEN = 2,
  FULL = 3
};

Charge level();

float voltage();


void init();
void shutdown();
void initGPIO();
void initADC();

}
}
}

#endif

