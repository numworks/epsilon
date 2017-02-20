#include <escher/timer.h>

Timer::Timer(Invocation invocation, uint32_t period) :
  m_invocation(invocation),
  m_period(period),
  m_numberOfTicksBeforeFire(period)
{
}

void Timer::tick() {
  m_numberOfTicksBeforeFire--;
  if (m_numberOfTicksBeforeFire == 0) {
    fire();
    reset();
  }
}

void Timer::reset() {
  m_numberOfTicksBeforeFire = m_period;
}

void Timer::fire() {
  m_invocation.perform(this);
}
