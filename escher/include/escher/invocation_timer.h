#ifndef ESCHER_INVOCATION_TIMER_H
#define ESCHER_INVOCATION_TIMER_H

#include <escher/timer.h>
#include <escher/invocation.h>

class InvocationTimer : public Timer {
public:
  InvocationTimer(Invocation invocation, uint32_t period);
private:
  void fire() override;
  Invocation m_invocation;
};

#endif
