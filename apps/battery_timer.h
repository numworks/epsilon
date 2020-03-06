#ifndef APPS_BATTERY_TIMER_H
#define APPS_BATTERY_TIMER_H

#include <escher.h>

class BatteryTimer : public Timer {
public:
  BatteryTimer();
private:
  bool fire() override;
};

#endif

