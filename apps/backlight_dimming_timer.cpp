#include "backlight_dimming_timer.h"

bool BacklightDimmingTimer::fire() {
  Ion::Backlight::setBrightness(k_dimBacklightBrightness);
  return false;
}
