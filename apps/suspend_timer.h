#ifndef APPS_SUSPEND_TIMER_H
#define APPS_SUSPEND_TIMER_H

#include <escher/timer.h>

class SuspendTimer : public Escher::Timer {
public:
  SuspendTimer();
private:
  constexpr static int k_idleBeforeSuspendDuration = 5*60*1000; // In miliseconds
  bool fire() override;
};

#endif

