#ifndef APPS_BATTERY_TIMER_H
#define APPS_BATTERY_TIMER_H

#include <escher.h>

class AppsContainer;

class BatteryTimer : public Timer {
public:
  BatteryTimer(AppsContainer * container);
private:
  void fire() override;
  AppsContainer * m_container;
};

#endif

