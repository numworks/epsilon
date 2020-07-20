#include <ion/battery.h>

#include "common.h"

bool Ion::Battery::isCharging() {
  return Ion::Simulator::CommonDriver::isCharging();
}

Ion::Battery::Charge Ion::Battery::level() {
  return Ion::Simulator::CommonDriver::getLevel();
}

float Ion::Battery::voltage() {
  return 0.0f;
}
