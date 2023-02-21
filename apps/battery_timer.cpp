#include "battery_timer.h"

#include "apps_container.h"

BatteryTimer::BatteryTimer() : Timer(1) {}

bool BatteryTimer::fire() {
  AppsContainer* container = AppsContainer::sharedAppsContainer();
  bool needRedrawing = container->updateBatteryState();
  if (Ion::Battery::level() == Ion::Battery::Charge::EMPTY &&
      !Ion::USB::isPlugged()) {
    container->shutdownDueToLowBattery();
  }
  return needRedrawing;
}
