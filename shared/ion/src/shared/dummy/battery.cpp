#include <ion/battery.h>

bool Ion::Battery::isCharging() { return false; }

Ion::Battery::Charge Ion::Battery::level(bool forceLevelUpdate) {
  return Charge::FULL;
}

float Ion::Battery::voltage() { return 4.2f; }
