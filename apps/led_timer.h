#ifndef APPS_LED_TIMER_H
#define APPS_LED_TIMER_H

#include <escher.h>

class LedTimer final : public Timer {
public:
  constexpr LedTimer() : Timer(1), m_on(false) {}
private:
  bool fire() override;
  bool m_on;
};

#endif

