#include "global_preferences.h"

GlobalPreferences * GlobalPreferences::sharedGlobalPreferences() {
  static GlobalPreferences globalPreferences;
  return &globalPreferences;
}

GlobalPreferences::ExamMode GlobalPreferences::examMode() const {
  if (m_examMode == ExamMode::Unknown) {
    uint8_t mode = Ion::ExamMode::FetchExamMode();
    assert(mode >= 0 && mode < 3); // mode can be cast in ExamMode (Off, Standard or Dutch)
    m_examMode = (ExamMode)mode;
  }
  return m_examMode;
}

void GlobalPreferences::setExamMode(ExamMode mode) {
  ExamMode currentMode = examMode();
  if (currentMode == mode) {
    return;
  }
  assert(mode != ExamMode::Unknown);
  int8_t deltaMode = (int8_t)mode - (int8_t)currentMode;
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
