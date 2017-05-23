#include "led_timer.h"

LedTimer::LedTimer() :
  Timer(1),
  m_on(false)
{
}

bool LedTimer::fire() {
  m_on = !m_on;
  KDColor ledColor = m_on ? KDColorRed : KDColorBlack;
  Ion::LED::setColor(ledColor);
  return false;
}
