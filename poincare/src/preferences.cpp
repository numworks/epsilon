#include <poincare/preferences.h>
#include <ion/include/ion/persisting_bytes.h>
#include <assert.h>

namespace Poincare {

constexpr int Preferences::DefaultNumberOfPrintedSignificantDigits;
constexpr int Preferences::VeryLargeNumberOfSignificantDigits;
constexpr int Preferences::LargeNumberOfSignificantDigits;
constexpr int Preferences::MediumNumberOfSignificantDigits;
constexpr int Preferences::ShortNumberOfSignificantDigits;
constexpr int Preferences::VeryShortNumberOfSignificantDigits;

Preferences::Preferences() :
  m_angleUnit(AngleUnit::Radian),
  m_displayMode(Preferences::PrintFloatMode::Decimal),
  m_editionMode(EditionMode::Edition2D),
  m_complexFormat(Preferences::ComplexFormat::Real),
  m_numberOfSignificantDigits(Preferences::DefaultNumberOfPrintedSignificantDigits),
  m_examMode(ExamMode::Unknown)
{}

Preferences * Preferences::sharedPreferences() {
  static Preferences preferences;
  return &preferences;
}

void Preferences::updateExamModeFromPersistingBytesIfNeeded() const {
  if (m_examMode == ExamMode::Unknown) {
    /* Persisting bytes are initialized at 0xFF but we set the first two ones at
    * 0 in shared_kernel_flash.ld to ensure the exam mode is in off position
    * after flashing. */
    uint8_t mode = Ion::PersistingBytes::read(k_examModePersistingByteIndex);
    // mode can be cast in ExamMode (Off, Standard, Dutch or Press-to-test)
    assert(mode >= static_cast<uint8_t>(ExamMode::Off) && mode < static_cast<uint8_t>(ExamMode::Undefined));
    m_examMode = static_cast<ExamMode>(mode);
    assert(m_examMode != ExamMode::Unknown);

    uint8_t params = Ion::PersistingBytes::read(k_pressToTestParamsPersistingByteIndex);
    assert(m_examMode == ExamMode::PressToTest || params == k_inactivePressToTest.m_value);
    m_pressToTestParams = PressToTestParams({params});
  }
}

Preferences::ExamMode Preferences::examMode() const {
  updateExamModeFromPersistingBytesIfNeeded();
  return m_examMode;
}

Preferences::PressToTestParams Preferences::pressToTestParams() const {
  updateExamModeFromPersistingBytesIfNeeded();
  return m_pressToTestParams;
}

void Preferences::setExamMode(ExamMode mode) {
  assert(mode != ExamMode::Unknown && mode < ExamMode::Undefined);
  ExamMode currentMode = examMode();
  if (currentMode == mode) {
    return;
  }
  Ion::PersistingBytes::write(static_cast<uint8_t>(mode), k_examModePersistingByteIndex);
  m_examMode = mode;
}

void Preferences::setPressToTestParams(PressToTestParams newPressToTestParams) {
  assert(m_examMode == ExamMode::PressToTest || newPressToTestParams.m_value == k_inactivePressToTest.m_value);
  PressToTestParams currentPressToTestParams = pressToTestParams();
  if (currentPressToTestParams.m_value == newPressToTestParams.m_value) {
    return;
  }
  Ion::PersistingBytes::write(newPressToTestParams.m_value, k_pressToTestParamsPersistingByteIndex);
  m_pressToTestParams = newPressToTestParams;
}

bool Preferences::equationSolverIsForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_equationSolverIsForbidden);
  return pressToTestParams().m_equationSolverIsForbidden;
}

bool Preferences::inequalityGraphingIsForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_inequalityGraphingIsForbidden);
  return pressToTestParams().m_inequalityGraphingIsForbidden;
}

bool Preferences::implicitPlotsAreForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_implicitPlotsAreForbidden);
  return pressToTestParams().m_implicitPlotsAreForbidden || examMode() == ExamMode::IBTest;
}

bool Preferences::statsDiagnosticsAreForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_statsDiagnosticsAreForbidden);
  return pressToTestParams().m_statsDiagnosticsAreForbidden;
}

bool Preferences::vectorProductsAreForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_vectorsAreForbidden);
  return pressToTestParams().m_vectorsAreForbidden || examMode() == ExamMode::IBTest;
}

bool Preferences::vectorNormIsForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_vectorsAreForbidden);
  return pressToTestParams().m_vectorsAreForbidden;
}

bool Preferences::basedLogarithmIsForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_basedLogarithmIsForbidden);
  return pressToTestParams().m_basedLogarithmIsForbidden;
}

bool Preferences::sumIsForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_sumIsForbidden);
  return pressToTestParams().m_sumIsForbidden;
}

bool Preferences::exactResultsAreForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_exactResultsAreForbidden);
  return pressToTestParams().m_exactResultsAreForbidden || examMode() == ExamMode::Dutch;
}

}
