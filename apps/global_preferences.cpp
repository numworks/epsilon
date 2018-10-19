#include "global_preferences.h"

GlobalPreferences * GlobalPreferences::sharedGlobalPreferences() {
  static GlobalPreferences globalPreferences;
  return &globalPreferences;
}

I18n::Language GlobalPreferences::language() const {
  return m_language;
}

void GlobalPreferences::setLanguage(I18n::Language language) {
  m_language = language;
}

GlobalPreferences::ExamMode GlobalPreferences::examMode() const {
  return m_examMode;
}

void GlobalPreferences::setExamMode(ExamMode examMode) {
  m_examMode = examMode;
}

#ifdef EPSILON_BOOT_PROMPT

void GlobalPreferences::setShowPopUp(bool showPopUp) {
  m_showPopUp = showPopUp;
}

#endif

int GlobalPreferences::brightnessLevel() const {
  return m_brightnessLevel;
}

void GlobalPreferences::setBrightnessLevel(int brightnessLevel) {
  if (m_brightnessLevel != brightnessLevel) {
    brightnessLevel = brightnessLevel < 0 ? 0 : brightnessLevel;
    brightnessLevel = brightnessLevel > Ion::Backlight::MaxBrightness ? Ion::Backlight::MaxBrightness : brightnessLevel;
    m_brightnessLevel = brightnessLevel;
    Ion::Backlight::setBrightness(m_brightnessLevel);
  }
}
