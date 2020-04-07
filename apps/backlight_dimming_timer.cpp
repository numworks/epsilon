#include "backlight_dimming_timer.h"

BacklightDimmingTimer::BacklightDimmingTimer() :
  Timer(GlobalPreferences::sharedGlobalPreferences()->idleBeforeDimmingSeconds()*1000/Timer::TickDuration)
{
}

bool BacklightDimmingTimer::fire() {
  Ion::Backlight::setBrightness(GlobalPreferences::sharedGlobalPreferences()->dimBacklightBrightness());
  return false;
}
