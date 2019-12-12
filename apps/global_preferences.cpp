#include "global_preferences.h"

GlobalPreferences * GlobalPreferences::sharedGlobalPreferences() {
  static GlobalPreferences globalPreferences;
  return &globalPreferences;
}

GlobalPreferences::ExamMode GlobalPreferences::examMode() const {
  if (m_examMode == ExamMode::Unknown) {
    m_examMode = (ExamMode)Ion::ExamMode::FetchExamMode();
  }
  assert((int)m_examMode >= 0 && (int)m_examMode <= 2);
  return m_examMode;
}

void GlobalPreferences::setExamMode(ExamMode mode) {
  if (examMode() == mode) {
    return;
  }
  assert(mode != ExamMode::Unknown);
  int8_t deltaMode = (int8_t)mode - (int8_t)examMode();
  deltaMode = deltaMode < 0 ? deltaMode + 3 : deltaMode;
  assert(deltaMode > 0);
  Ion::ExamMode::IncrementExamMode(deltaMode);
  m_examMode = mode;
}

void GlobalPreferences::setBrightnessLevel(int brightnessLevel) {
  if (m_brightnessLevel != brightnessLevel) {
    brightnessLevel = brightnessLevel < 0 ? 0 : brightnessLevel;
    brightnessLevel = brightnessLevel > Ion::Backlight::MaxBrightness ? Ion::Backlight::MaxBrightness : brightnessLevel;
    m_brightnessLevel = brightnessLevel;
    Ion::Backlight::setBrightness(m_brightnessLevel);
  }
}
