#include <escher/timer.h>

bool Timer::tick() {
  m_numberOfTicksBeforeFire--;
  if (m_numberOfTicksBeforeFire == 0) {
    bool needRedraw = fire();
    reset();
    return needRedraw;
  }
  return false;
}
