#include "global_preferences.h"

GlobalPreferences::GlobalPreferences() :
  m_language(I18n::Language::EN),
  m_examMode(ExamMode::Desactivate),
#if EPSILON_SOFTWARE_UPDATE_PROMPT
  m_showPopUp(true),
#endif
  m_brightnessLevel(Ion::Backlight::MaxBrightness)
{
}

GlobalPreferences * GlobalPreferences::sharedGlobalPreferences() {
  static GlobalPreferences globalPreferences;
  return &globalPreferences;
}

I18n::Language GlobalPreferences::language() const {
  return m_language;
}

void GlobalPreferences::setLanguage(I18n::Language language) {
  if (language != m_language) {
    m_language = language;
  }
}

GlobalPreferences::ExamMode GlobalPreferences::examMode() const {
  return m_examMode;
}

void GlobalPreferences::setExamMode(ExamMode examMode) {
  if (examMode != m_examMode) {
    m_examMode = examMode;
  }
}

#if EPSILON_SOFTWARE_UPDATE_PROMPT
bool GlobalPreferences::showPopUp() const {
  return m_showPopUp;
}

void GlobalPreferences::setShowPopUp(bool showPopUp) {
  if (showPopUp != m_showPopUp) {
    m_showPopUp = showPopUp;
  }
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
