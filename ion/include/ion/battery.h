#ifndef ION_BATTERY_H
#define ION_BATTERY_H

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
namespace Battery {

bool isCharging();

enum class Charge : uint8_t {
  EMPTY = 0,
  P20 = 1,  //  20%
  P40 = 2,  //  40%
  P60 = 3,  //  60%
  P80 = 4,  //  80%
  FULL = 5  // 100%
};
constexpr int numberOfChargeStates = 6;
Charge level();

float voltage();

}  // namespace Battery
}  // namespace Ion

#endif
