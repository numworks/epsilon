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
    // mode can be cast in ExamMode (Off, Standard, Dutch or Press-to-test)
    assert(mode >= static_cast<uint8_t>(ExamMode::Off) && mode <= static_cast<uint8_t>(ExamMode::PressToTest));
    m_examMode = static_cast<ExamMode>(mode);
    updatePoincareSharedPreferences();
  }
  return m_examMode;
}

GlobalPreferences::PressToTestParams GlobalPreferences::pressToTestParams() const {
  if (m_pressToTestParams.m_isUnknown) {
    /* Persisting bytes are initialized at 0xFF but we set the first two ones at
     * 0 in shared_kernel_flash.ld to ensure pressToTestParams are false after
     * flashing. */
    PressToTestParams newPressToTestParams = {Ion::PersistingBytes::read(k_pressToTestParamsPersistingByteIndex)};
    assert(!newPressToTestParams.m_isUnknown);
    m_pressToTestParams = newPressToTestParams;
    updatePoincareSharedPreferences();
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
  if (currentPressToTestParams.m_value == newPressToTestParams.m_value) {
    return;
  }
  assert(!newPressToTestParams.m_isUnknown);
  Ion::PersistingBytes::write(newPressToTestParams.m_value, k_pressToTestParamsPersistingByteIndex);
  m_pressToTestParams = newPressToTestParams;
  updatePoincareSharedPreferences();
}

void GlobalPreferences::setBrightnessLevel(int brightnessLevel) {
  if (m_brightnessLevel != brightnessLevel) {
    brightnessLevel = brightnessLevel < 0 ? 0 : brightnessLevel;
    brightnessLevel = brightnessLevel > Ion::Backlight::MaxBrightness ? Ion::Backlight::MaxBrightness : brightnessLevel;
    m_brightnessLevel = brightnessLevel;
    Ion::Backlight::setBrightness(m_brightnessLevel);
  }
}

bool GlobalPreferences::equationSolverIsForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_equationSolverIsForbidden);
  return pressToTestParams().m_equationSolverIsForbidden;
}

bool GlobalPreferences::inequalityGraphingIsForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_inequalityGraphingIsForbidden);
  return pressToTestParams().m_inequalityGraphingIsForbidden;
}

bool GlobalPreferences::implicitPlotsAreForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_implicitPlotsAreForbidden);
  return pressToTestParams().m_implicitPlotsAreForbidden;
}

bool GlobalPreferences::statsDiagnosticsAreForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_statsDiagnosticsAreForbidden);
  return pressToTestParams().m_statsDiagnosticsAreForbidden;
}

bool GlobalPreferences::vectorsAreForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_vectorsAreForbidden);
  return pressToTestParams().m_vectorsAreForbidden;
}

bool GlobalPreferences::basedLogarithmIsForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_basedLogarithmIsForbidden);
  return pressToTestParams().m_basedLogarithmIsForbidden;
}

bool GlobalPreferences::sumIsForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_sumIsForbidden);
  return pressToTestParams().m_sumIsForbidden;
}

void GlobalPreferences::updatePoincareSharedPreferences() const {
  /* These press-to-test parameters are stored as Poincare::Preferences to keep
   * Poincare independent from the apps. */
  Poincare::Preferences::sharedPreferences()->setBasedLogarithmIsForbidden(basedLogarithmIsForbidden());
  Poincare::Preferences::sharedPreferences()->setSumIsForbidden(sumIsForbidden());
  Poincare::Preferences::sharedPreferences()->setVectorsAreForbidden(vectorsAreForbidden());
}
