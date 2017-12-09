#include "battery_timer.h"
#include "apps_container.h"

bool BatteryTimer::fire() {
  bool needRedrawing = m_container->updateBatteryState();
  if (Ion::Battery::level() == Ion::Battery::Charge::EMPTY) {
    m_container->shutdownDueToLowBattery();
  }
  return needRedrawing;
}
