#ifndef APPS_SUSPEND_TIMER_H
#define APPS_SUSPEND_TIMER_H

#include <escher.h>

class AppsContainer;

class SuspendTimer final : public Timer {
public:
  constexpr SuspendTimer(AppsContainer * container) : Timer(k_idleBeforeSuspendDuration/Timer::TickDuration), m_container(container) {}
private:
  constexpr static int k_idleBeforeSuspendDuration = 5*60*1000; // In miliseconds
  bool fire() override;
  AppsContainer * m_container;
};

#endif

