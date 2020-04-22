#ifndef APPS_BACKLIGHT_DIMMING_TIMER_H
#define APPS_BACKLIGHT_DIMMING_TIMER_H

#include <escher.h>
#include "global_preferences.h"

class BacklightDimmingTimer : public Timer {
public:
  BacklightDimmingTimer();
private:
  bool fire() override;
};

#endif

