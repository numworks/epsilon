#include "battery_timer.h"
#include "apps_container.h"

BatteryTimer::BatteryTimer(AppsContainer * container) :
  Timer(5),
  m_container(container)
{
}

void BatteryTimer::fire() {
  m_container->updateBatteryState();
  if (Ion::Battery::level() == Ion::Battery::Charge::EMPTY) {
    m_container->shutdownDueToLowBattery();
  }
}
