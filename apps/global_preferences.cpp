#include "global_preferences.h"

GlobalPreferences * GlobalPreferences::sharedGlobalPreferences() {
  static GlobalPreferences globalPreferences;
  return &globalPreferences;
}

void GlobalPreferences::setBrightnessLevel(int brightnessLevel) {
  if (m_brightnessLevel != brightnessLevel) {
    brightnessLevel = brightnessLevel < 0 ? 0 : brightnessLevel;
    brightnessLevel = brightnessLevel > Ion::Backlight::MaxBrightness ? Ion::Backlight::MaxBrightness : brightnessLevel;
    m_brightnessLevel = brightnessLevel;
    Ion::Backlight::setBrightness(m_brightnessLevel);
  }
}

void GlobalPreferences::setDimBacklightBrightness(int dimBacklightBrightness) {
  if (m_dimBacklightBrightness != dimBacklightBrightness) {
    dimBacklightBrightness = dimBacklightBrightness < 0 ? 0 : dimBacklightBrightness;
    dimBacklightBrightness = dimBacklightBrightness > Ion::Backlight::MaxBrightness ? Ion::Backlight::MaxBrightness : dimBacklightBrightness;
    m_dimBacklightBrightness = dimBacklightBrightness;
  }
}

void GlobalPreferences::setIdleBeforeSuspendSeconds(int idleBeforeSuspendSeconds) {
  if (m_idleBeforeSuspendSeconds != idleBeforeSuspendSeconds) {
    idleBeforeSuspendSeconds = idleBeforeSuspendSeconds < 5 ? 5 : idleBeforeSuspendSeconds;
    idleBeforeSuspendSeconds = idleBeforeSuspendSeconds > 7200 ? 7200 : idleBeforeSuspendSeconds;
    m_idleBeforeSuspendSeconds = idleBeforeSuspendSeconds;
  }
}

void GlobalPreferences::setIdleBeforeDimmingSeconds(int idleBeforeDimmingSeconds) {
  if (m_idleBeforeDimmingSeconds != idleBeforeDimmingSeconds) {
    idleBeforeDimmingSeconds = idleBeforeDimmingSeconds < 3 ? 3 : idleBeforeDimmingSeconds;
    idleBeforeDimmingSeconds = idleBeforeDimmingSeconds > 1200 ? 1200 : idleBeforeDimmingSeconds;
    m_idleBeforeDimmingSeconds = idleBeforeDimmingSeconds;
  }
}
