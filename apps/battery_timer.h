#ifndef APPS_BATTERY_TIMER_H
#define APPS_BATTERY_TIMER_H

#include <escher.h>

class AppsContainer;

class BatteryTimer final : public Timer {
public:
  constexpr BatteryTimer(AppsContainer * container) : Timer(1), m_container(container) {}
private:
  bool fire() override;
  AppsContainer * m_container;
};

#endif

