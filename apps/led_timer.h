#ifndef APPS_LED_TIMER_H
#define APPS_LED_TIMER_H

#include <escher.h>

class LedTimer : public Timer {
public:
  LedTimer();
private:
  void fire() override;
  bool m_on;
};

#endif

