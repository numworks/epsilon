#include "global_preferences.h"

GlobalPreferences * GlobalPreferences::sharedGlobalPreferences() {
  static GlobalPreferences globalPreferences;
  return &globalPreferences;
}

bool GlobalPreferences::examMode() const {
  if (m_examMode == ExamMode::Unknown) {
    m_examMode = (ExamMode)Ion::ExamMode::FetchExamMode();
  }
  assert((int)m_examMode == 0 || (int)m_examMode == 1);
  return (bool)m_examMode;
}

void GlobalPreferences::setExamMode(bool activateExamMode) {
  if (((bool)examMode()) == activateExamMode) {
    return;
  }
  Ion::ExamMode::ToggleExamMode();
  m_examMode = (ExamMode)activateExamMode;
}

void GlobalPreferences::setBrightnessLevel(int brightnessLevel) {
  if (m_brightnessLevel != brightnessLevel) {
    brightnessLevel = brightnessLevel < 0 ? 0 : brightnessLevel;
    brightnessLevel = brightnessLevel > Ion::Backlight::MaxBrightness ? Ion::Backlight::MaxBrightness : brightnessLevel;
    m_brightnessLevel = brightnessLevel;
    Ion::Backlight::setBrightness(m_brightnessLevel);
  }
}
