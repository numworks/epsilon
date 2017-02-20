#include <escher/invocation_timer.h>

InvocationTimer::InvocationTimer(Invocation invocation, uint32_t period) :
  Timer(period),
  m_invocation(invocation)
{
}

void InvocationTimer::fire() {
  m_invocation.perform(this);
}
