#ifndef APPS_BACKLIGHT_DIMMING_TIMER_H
#define APPS_BACKLIGHT_DIMMING_TIMER_H

#include <escher/timer.h>

class BacklightDimmingTimer : public Escher::Timer {
 public:
  BacklightDimmingTimer();

 private:
  constexpr static int k_idleBeforeDimmingDuration =
      30 * 1000;  // In miliseconds
  constexpr static int k_dimBacklightBrightness = 0;
  bool fire() override;
};

#endif
