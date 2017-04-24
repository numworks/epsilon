#include "backlight_dimming_timer.h"

BacklightDimmingTimer::BacklightDimmingTimer() :
  Timer(k_idleBeforeDimmingDuration/Timer::TickDuration)
{
}

void BacklightDimmingTimer::fire() {
  Ion::Backlight::setBrightness(k_dimBacklightBrightness);
}
