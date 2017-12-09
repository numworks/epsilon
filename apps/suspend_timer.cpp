#include "suspend_timer.h"
#include "apps_container.h"

bool SuspendTimer::fire() {
  m_container->suspend();
  return false;
}
