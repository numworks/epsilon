#include <poincare/preferences.h>
#include <poincare/expression.h>
#include <poincare/unit.h>
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

Preferences Preferences::ClonePreferencesWithNewComplexFormat(ComplexFormat complexFormat, Preferences * preferences) {
  Preferences result = *preferences;
  result.setComplexFormat(complexFormat);
  return result;
}

Preferences::ComplexFormat Preferences::UpdatedComplexFormatWithExpressionInput(ComplexFormat complexFormat, const Expression & exp, Context * context) {
  if ((complexFormat == ComplexFormat::Real) && exp.hasComplexI(context)) {
    return k_defautComplexFormatIfNotReal;
  }
  return complexFormat;
}

void Preferences::updateExamModeFromPersistingBytesIfNeeded() const {
  assert(ExamPersistingBytes(0xffff).mode() == ExamMode::Unknown);
  if (m_examMode == ExamMode::Unknown) {
    ExamPersistingBytes pb(Ion::PersistingBytes::read());
#if PLATFORM_DEVICE
    static_assert(sizeof(pb) == sizeof(uint16_t), "Exam mode encoding on persisting bytes has changed.");
#endif
    ExamMode mode = pb.mode();
    assert(static_cast<uint8_t>(mode) >= static_cast<uint8_t>(ExamMode::Off) && static_cast<uint8_t>(mode) < static_cast<uint8_t>(ExamMode::Undefined));

    if (mode == ExamMode::Unknown) {
      /* PersistingBytes are invalid, most likely because of a botched update
       * with a version that supports a different exam mode encoding. */
      m_examMode = ExamMode::Off;
      m_pressToTestParams = k_inactivePressToTest;
    } else {
      m_examMode = mode;
      m_pressToTestParams = pb.params();
    }
    assert(m_examMode != ExamMode::Unknown
        && m_examMode != ExamMode::Undefined
        && (m_examMode == ExamMode::PressToTest || m_pressToTestParams == k_inactivePressToTest));
  } else {
    assert(ExamPersistingBytes(m_examMode, m_pressToTestParams).m_value == Ion::PersistingBytes::read());
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
  assert(mode == ExamMode::PressToTest || newPressToTestParams == k_inactivePressToTest);
  ExamMode currentMode = examMode();
  PressToTestParams currentPressToTestParams = pressToTestParams();
  if (currentMode == mode && currentPressToTestParams == newPressToTestParams) {
    return;
  }
  ExamPersistingBytes newPB(mode, newPressToTestParams);
  Ion::PersistingBytes::write(newPB.m_value);
  m_examMode = mode;
  m_pressToTestParams = newPressToTestParams;
}

bool Preferences::elementsAppIsForbidden() const {
  assert(examMode() == ExamMode::PressToTest || !pressToTestParams().m_elementsAppIsForbidden);
  return pressToTestParams().m_elementsAppIsForbidden;
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
