#include "backlight_dimming_timer.h"
#include "global_preferences.h"
#include <ion/backlight.h>
#include <ion/events.h>
#include <apps/apps_container.h>

BacklightDimmingTimer::BacklightDimmingTimer() :
  Timer(GlobalPreferences::sharedGlobalPreferences()->idleBeforeDimmingSeconds()*1000/Timer::TickDuration)
{
}

bool BacklightDimmingTimer::fire(){
  int i = Ion::Backlight::brightness();
  while (i > 0){
    int t = 20;
    Ion::Events::Event e = Ion::Events::getEvent(&t);
    AppsContainer::sharedAppsContainer()->dispatchEvent(e);
    if (e.isKeyboardEvent()){
      return false;
    }

    Ion::Backlight::setBrightness(i);
    i -= 15;
  }
  return false;
}
