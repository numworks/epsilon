#ifndef APPS_CLOCK_TIMER_H
#define APPS_CLOCK_TIMER_H

#include <escher.h>

class AppsContainer;

class ClockTimer : public Timer {
public:
  ClockTimer(AppsContainer * container);
private:
  bool fire() override;
  AppsContainer * m_container;
};

#endif
