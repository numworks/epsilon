#include "backlight_dimming_timer.h"

#include <ion.h>

BacklightDimmingTimer::BacklightDimmingTimer()
    : Timer(k_idleBeforeDimmingDuration / Timer::TickDuration) {}

bool BacklightDimmingTimer::fire() {
  Ion::Backlight::setBrightness(k_dimBacklightBrightness);
  return false;
}
