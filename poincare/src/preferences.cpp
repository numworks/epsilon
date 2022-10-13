#include <poincare/preferences.h>
#include <poincare/expression.h>
#include <poincare/unit.h>
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

Preferences Preferences::PreferencesClone(Preferences * preferences) {
  Preferences result = Preferences();
  result.setAngleUnit(preferences->angleUnit());
  result.setDisplayMode(preferences->displayMode());
  result.setEditionMode(preferences->editionMode());
  result.setComplexFormat(preferences->complexFormat());
  result.setCombinatoricSymbols(preferences->combinatoricSymbols());
  result.setNumberOfSignificantDigits(preferences->numberOfSignificantDigits());
  result.enableMixedFractions(static_cast<MixedFractions>(preferences->mixedFractionsAreEnabled()));
  result.setExamMode(preferences->examMode(), preferences->pressToTestParams());
  return result;
}

Preferences Preferences::ClonePreferencesWithNewComplexFormatAndAngleUnit(ComplexFormat complexFormat, AngleUnit angleUnit, Preferences * preferences) {
  Preferences result = PreferencesClone(preferences);
  result.setComplexFormat(complexFormat);
  result.setAngleUnit(angleUnit);
  return result;
}

Preferences::ComplexFormat Preferences::UpdatedComplexFormatWithExpressionInput(ComplexFormat complexFormat, const Expression & exp, Context * context) {
  if ((complexFormat == ComplexFormat::Real) && exp.hasComplexI(context)) {
    return k_defautComplexFormatIfNotReal;
  }
  return complexFormat;
}

Preferences::AngleUnit Preferences::UpdatedAngleUnitWithExpressionInput(AngleUnit angleUnit, const Expression & exp, Context * context, bool * forceChange) {
  struct AngleInformations {
    bool hasTrigonometry;
    bool hasRadians;
    bool hasDegrees;
    bool hasGradians;
  };
  AngleInformations angleInformations = {};
  exp.recursivelyMatches(
    [](const Expression e, Context * context, void * data) {
      AngleInformations * angleInformations = static_cast<AngleInformations*>(data);
      angleInformations->hasTrigonometry = angleInformations->hasTrigonometry || e.isOfType({
          ExpressionNode::Type::Sine, ExpressionNode::Type::Cosine, ExpressionNode::Type::Tangent,
          ExpressionNode::Type::Secant, ExpressionNode::Type::Cosecant, ExpressionNode::Type::Cotangent
        });
      if (e.type() != ExpressionNode::Type::Unit) {
        return TrinaryBoolean::Unknown;
      }
      const Unit::Representative * representative = static_cast<const Unit &>(e).representative();
      angleInformations->hasRadians = angleInformations->hasRadians || representative == &Unit::k_angleRepresentatives[Unit::k_radianRepresentativeIndex];
      angleInformations->hasGradians = angleInformations->hasGradians || representative == &Unit::k_angleRepresentatives[Unit::k_gradianRepresentativeIndex];
      angleInformations->hasDegrees =
        angleInformations->hasDegrees
        || representative == &Unit::k_angleRepresentatives[Unit::k_degreeRepresentativeIndex]
        || representative == &Unit::k_angleRepresentatives[Unit::k_arcMinuteRepresentativeIndex]
        || representative == &Unit::k_angleRepresentatives[Unit::k_arcSecondRepresentativeIndex];
      return TrinaryBoolean::Unknown;
    },
    context,
    ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, static_cast<void*>(&angleInformations));

  bool forceChangeResult = false;
  Preferences::AngleUnit result = angleUnit;
  if (!angleInformations.hasTrigonometry && (angleInformations.hasDegrees + angleInformations.hasRadians + angleInformations.hasGradians == 1))
  {
    // Exactly one among hasDegree, hasRadians and hasGradians is true
    forceChangeResult = true;
    result = angleInformations.hasDegrees ? Preferences::AngleUnit::Degree : (angleInformations.hasGradians ? Preferences::AngleUnit::Gradian : Preferences::AngleUnit::Radian);
  }
  if (forceChange) {
    *forceChange = forceChangeResult;
  }
  return result;
}

void Preferences::updateExamModeFromPersistingBytesIfNeeded() const {
  if (m_examMode == ExamMode::Unknown) {
    Ion::PersistingBytes::PersistingBytesInt pb = Ion::PersistingBytes::read();
    static_assert(sizeof(pb) == sizeof(uint16_t), "Exam mode encoding on persisting bytes has changed.");
    uint8_t params = pb & 0xFF;
    uint8_t mode = pb >> 8;
    assert(mode >= static_cast<uint8_t>(ExamMode::Off) && mode < static_cast<uint8_t>(ExamMode::Undefined));

    if (mode == static_cast<uint8_t>(ExamMode::Unknown)) {
      /* PersistingBytes are invalid, most likely because of a botched update
       * with a version that supports a different exam mode encoding. */
      m_examMode = ExamMode::Off;
      m_pressToTestParams = k_inactivePressToTest;
    } else {
      m_examMode = static_cast<ExamMode>(mode);
      m_pressToTestParams = PressToTestParams({params});
    }
    assert(m_examMode != ExamMode::Unknown
        && m_examMode != ExamMode::Undefined
        && (m_examMode == ExamMode::PressToTest || params == k_inactivePressToTest.m_value));
  } else {
    assert((m_pressToTestParams.m_value | (static_cast<uint8_t>(m_examMode) << 8)) == Ion::PersistingBytes::read());
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
  Ion::PersistingBytes::write(newPressToTestParams.m_value | (static_cast<uint8_t>(mode) << 8));
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
