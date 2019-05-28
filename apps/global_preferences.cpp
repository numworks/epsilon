#include "global_preferences.h"

static inline int minInt(int x, int y) { return x < y ? x : y; }
static inline int maxInt(int x, int y) { return x > y ? x : y; }

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

void GlobalPreferences::setAccessibilityGammaRed(int level) {
  m_accessibilityGammaRed = minInt(maxInt(level, 0), NumberOfGammaStates);
}

void GlobalPreferences::setAccessibilityGammaGreen(int level) {
  m_accessibilityGammaGreen = minInt(maxInt(level, 0), NumberOfGammaStates);
}

void GlobalPreferences::setAccessibilityGammaBlue(int level) {
  m_accessibilityGammaBlue = minInt(maxInt(level, 0), NumberOfGammaStates);
}
