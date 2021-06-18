#include "clock_timer.h"
#include "apps_container.h"

ClockTimer::ClockTimer(AppsContainer * container) :
  Timer(1),
  m_container(container)
{
}

bool ClockTimer::fire() {
  return m_container->updateClock();
}
