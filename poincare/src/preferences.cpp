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
    Ion::PersistingBytes::PersistingBytesInt pb = Ion::PersistingBytes::read();
    assert(sizeof(pb) == sizeof(uint16_t));
    uint8_t params = pb & 0xFF;
    uint8_t mode = pb >> 8;

    assert(mode >= static_cast<uint8_t>(ExamMode::Off) && mode < static_cast<uint8_t>(ExamMode::Undefined));
    m_examMode = static_cast<ExamMode>(mode);
    if (m_examMode == ExamMode::Unknown) {
      m_examMode = ExamMode::Off;
    }
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

void Preferences::setExamMode(ExamMode mode, PressToTestParams newPressToTestParams) {
  assert(mode != ExamMode::Unknown && mode < ExamMode::Undefined);
  assert(mode == ExamMode::PressToTest || newPressToTestParams.m_value == k_inactivePressToTest.m_value);
  ExamMode currentMode = examMode();
  PressToTestParams currentPressToTestParams = pressToTestParams();
  if (currentMode == mode && currentPressToTestParams.m_value == newPressToTestParams.m_value) {
    return;
  }
  Ion::PersistingBytes::write(currentPressToTestParams.m_value | (static_cast<uint8_t>(mode) << 8));
  m_examMode = mode;
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
