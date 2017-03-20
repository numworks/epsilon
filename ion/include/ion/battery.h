#ifndef ION_BATTERY_H
#define ION_BATTERY_H

/*
 *
 * RAIN = External input impedance should be <= to
 * (k-0.5)/(fADC * CADC * ln(2^(n+2)) - RADC
 *
 * n = resolution = 12
 * k = number of sampling periods
 */


namespace Ion {
namespace Battery {

bool isCharging();

enum class Charge {
  EMPTY,
  LOW,
  SOMEWHERE_INBETWEEN,
  FULL
};
Charge level();

float voltage();

}
}

#endif
