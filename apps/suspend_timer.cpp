#include "suspend_timer.h"
#include "apps_container.h"

SuspendTimer::SuspendTimer(AppsContainer * container) :
  Timer(k_idleBeforeSuspendDuration/Timer::TickDuration),
  m_container(container)
{
}

bool SuspendTimer::fire() {
  /* We could just call m_container->suspend(), but we want to notify all
   * responders in the responder chain that the calculator will be switched off,
   * so we use an event to switch off the calculator. */
  m_container->dispatchEvent(Ion::Events::OnOff);
  return false;
}
