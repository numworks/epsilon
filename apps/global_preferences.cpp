#include "global_preferences.h"

GlobalPreferences * GlobalPreferences::sharedGlobalPreferences() {
  static GlobalPreferences globalPreferences;
  return &globalPreferences;
}

GlobalPreferences::ExamMode GlobalPreferences::examMode() const {
  if (m_examMode == ExamMode::Unknown) {
    uint8_t mode = Ion::ExamMode::FetchExamMode();
    assert(mode >= 0 && mode < 5); // mode can be cast in ExamMode (Off, Standard, NoSym, Dutch or NoSymNoText)
    m_examMode = (ExamMode)mode;
  }
  return m_examMode;
}

GlobalPreferences::ExamMode GlobalPreferences::tempExamMode() const {
  return m_tempExamMode;
}


void GlobalPreferences::setExamMode(ExamMode mode) {
  ExamMode currentMode = examMode();
  if (currentMode == mode) {
    return;
  }
  assert(mode != ExamMode::Unknown);
  int8_t deltaMode = (int8_t)mode - (int8_t)currentMode;
  deltaMode = deltaMode < 0 ? deltaMode + 4 : deltaMode;
  assert(deltaMode > 0);
  Ion::ExamMode::IncrementExamMode(deltaMode);
  m_examMode = mode;
}

void GlobalPreferences::setTempExamMode(ExamMode mode) {
  m_tempExamMode = mode;
}

void GlobalPreferences::setBrightnessLevel(int brightnessLevel) {
  if (m_brightnessLevel != brightnessLevel) {
    brightnessLevel = brightnessLevel < 0 ? 0 : brightnessLevel;
    brightnessLevel = brightnessLevel > Ion::Backlight::MaxBrightness ? Ion::Backlight::MaxBrightness : brightnessLevel;
    m_brightnessLevel = brightnessLevel;
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

void GlobalPreferences::setBrightnessShortcut(int brightnessShortcut){
  m_brightnessShortcut = brightnessShortcut;
}
