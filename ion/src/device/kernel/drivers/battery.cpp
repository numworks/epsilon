#include "battery.h"
#include <drivers/config/battery.h>

namespace Ion {
namespace Device {
namespace Battery {

using namespace Regs;

bool isCharging() {
  return !Config::ChargingPin.group().IDR()->get(Config::ChargingPin.pin());
}

Ion::Battery::Charge level() {
  static Ion::Battery::Charge previousLevel = Ion::Battery::Charge::FULL;

  // Get the current voltage
  float currentVoltage = voltage();

  constexpr static int numberOfChargeStates = 4;
  constexpr static int numberOfThresholds = numberOfChargeStates - 1;
  constexpr float hysteresis = 0.02f;
  const float thresholds[numberOfThresholds] = {3.6f + hysteresis, 3.7f, 3.8f}; // We do not want to lower the threshold for empty battery, so we add the hysteresis to it
  int nextLevel = -1;
  for (int i = 0; i < numberOfThresholds; i++) {
    if (currentVoltage < thresholds[i] + hysteresis * (i < (int)previousLevel ? -1.0f : 1.0f)) {
      nextLevel = i;
      break;
    }
  }
  if (nextLevel < 0) {
    nextLevel = (int) Ion::Battery::Charge::FULL;
  }
  previousLevel = (Ion::Battery::Charge)nextLevel;
  return previousLevel;
}

}
}
}
