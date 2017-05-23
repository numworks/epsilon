#ifndef APPS_SUSPEND_TIMER_H
#define APPS_SUSPEND_TIMER_H

#include <escher.h>

class AppsContainer;

class SuspendTimer : public Timer {
public:
  SuspendTimer(AppsContainer * container);
private:
  constexpr static int k_idleBeforeSuspendDuration = 5*60*1000; // In miliseconds
  bool fire() override;
  AppsContainer * m_container;
};

#endif

