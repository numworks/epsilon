#include "global_preferences.h"
#include <ion/include/ion/persisting_bytes.h>

GlobalPreferences * GlobalPreferences::sharedGlobalPreferences() {
  static GlobalPreferences globalPreferences;
  return &globalPreferences;
}

GlobalPreferences::ExamMode GlobalPreferences::examMode() const {
  if (m_examMode == ExamMode::Unknown) {
    /* Persisting bytes are initialized at 0xFF but we set the first two ones at
     * 0 in shared_kernel_flash.ld to ensure the exam mode in off position after
     * flashing. */
    uint8_t mode = Ion::PersistingBytes::read(k_examModePersistingByteIndex);
    assert(mode >= 0 && mode < 3); // mode can be cast in ExamMode (Off, Standard or Dutch)
    m_examMode = static_cast<ExamMode>(mode);
  }
  return m_examMode;
}

GlobalPreferences::PressToTestParams GlobalPreferences::pressToTestParams() const {
  if (m_pressToTestParams.isUnknown) {
    /* Persisting bytes are initialized at 0xFF but we set the first two ones at
     * 0 in shared_kernel_flash.ld to ensure pressToTestParams are false after
     * flashing. */
    PressToTestParams newPressToTestParams = {Ion::PersistingBytes::read(k_pressToTestParamsPersistingByteIndex)};
    assert(!newPressToTestParams.isUnknown);
    m_pressToTestParams = newPressToTestParams;
  }
  return m_pressToTestParams;
}

void GlobalPreferences::setExamMode(ExamMode mode) {
  ExamMode currentMode = examMode();
  if (currentMode == mode) {
    return;
  }
  assert(mode != ExamMode::Unknown);
  Ion::PersistingBytes::write(static_cast<uint8_t>(mode), k_examModePersistingByteIndex);
  m_examMode = mode;
}

void GlobalPreferences::setPressToTestParams(PressToTestParams newPressToTestParams) {
  PressToTestParams currentPressToTestParams = pressToTestParams();
  if (currentPressToTestParams.value == newPressToTestParams.value) {
    return;
  }
  assert(!newPressToTestParams.isUnknown);
  Ion::PersistingBytes::write(newPressToTestParams.value, k_pressToTestParamsPersistingByteIndex);
  m_pressToTestParams = newPressToTestParams;
}

void GlobalPreferences::setBrightnessLevel(int brightnessLevel) {
  if (m_brightnessLevel != brightnessLevel) {
    brightnessLevel = brightnessLevel < 0 ? 0 : brightnessLevel;
    brightnessLevel = brightnessLevel > Ion::Backlight::MaxBrightness ? Ion::Backlight::MaxBrightness : brightnessLevel;
    m_brightnessLevel = brightnessLevel;
    Ion::Backlight::setBrightness(m_brightnessLevel);
  }
}
