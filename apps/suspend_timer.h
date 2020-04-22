#ifndef APPS_SUSPEND_TIMER_H
#define APPS_SUSPEND_TIMER_H

#include <escher.h>

class SuspendTimer : public Timer {
public:
  SuspendTimer();
private:
  bool fire() override;
};

#endif

