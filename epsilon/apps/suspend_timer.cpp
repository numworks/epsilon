#include "suspend_timer.h"

#include "apps_container.h"

SuspendTimer::SuspendTimer()
    : Timer(k_idleBeforeSuspendDuration / Timer::TickDuration) {}

bool SuspendTimer::fire() {
  Ion::Power::suspend();
  return false;
}
