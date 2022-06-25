#ifndef ESCHER_TIMER_MANAGER_H
#define ESCHER_TIMER_MANAGER_H

#include <escher/timer.h>

class TimerManager {
public:
  // We need this file to not create dependece between apps and escher.
  static void AddTimer(Timer * timer);
  static void RemoveTimer(Timer * timer);
};

#endif
