#include "backlight_dimming_timer.h"
#include "global_preferences.h"


BacklightDimmingTimer::BacklightDimmingTimer() :
  Timer(GlobalPreferences::sharedGlobalPreferences()->idleBeforeDimmingSeconds()*1000/Timer::TickDuration)  // In miliseconds
{
}

bool BacklightDimmingTimer::fire() {
  Ion::Backlight::setBrightness(GlobalPreferences::sharedGlobalPreferences()->dimBacklightBrightness());
  return false;
}
