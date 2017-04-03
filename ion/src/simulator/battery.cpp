#include <ion/battery.h>
#include <math.h>

bool Ion::Battery::isCharging() {
  return false;
}

Ion::Battery::Charge Ion::Battery::level() {
  return Charge::FULL;
}

float Ion::Battery::voltage() {
  return NAN;
}
