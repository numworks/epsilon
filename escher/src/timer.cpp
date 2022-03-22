#include <escher/timer.h>

Timer::Timer(uint32_t period) :
  m_period(period),
  m_numberOfTicksBeforeFire(period)
{
}

bool Timer::tick() {
  m_numberOfTicksBeforeFire--;
  if (m_numberOfTicksBeforeFire == 0) {
    bool needRedraw = fire();
    reset();
    return needRedraw;
  }
  return false;
}

void Timer::reset(uint32_t NewPeriod) {
  if(NewPeriod != -1){
    m_period = NewPeriod;
  }
  m_numberOfTicksBeforeFire = m_period;
}
