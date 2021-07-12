#include "global_preferences.h"
#include <ion/include/ion/persisting_bytes.h>

GlobalPreferences * GlobalPreferences::sharedGlobalPreferences() {
  static GlobalPreferences globalPreferences;
  return &globalPreferences;
}

GlobalPreferences::ExamMode GlobalPreferences::examMode() const {
  if (m_examMode == ExamMode::Unknown) {
    /* Persisting bytes are initialized at 0xFF so we use an ~operator the bits
     * to ensure the exam mode in off position after flashing. */
    uint8_t mode = Ion::PersistingBytes::read();
    assert(mode >= 0 && mode < 3); // mode can be cast in ExamMode (Off, Standard or Dutch)
    m_examMode = static_cast<ExamMode>(mode);
  }
  return m_examMode;
}

void GlobalPreferences::setExamMode(ExamMode mode) {
  ExamMode currentMode = examMode();
  if (currentMode == mode) {
    return;
  }
  assert(mode != ExamMode::Unknown);
  // Cf comment above
  Ion::PersistingBytes::write(static_cast<uint8_t>(mode));
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
