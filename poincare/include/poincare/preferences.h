#ifndef POINCARE_PREFERENCES_H
#define POINCARE_PREFERENCES_H

#include <assert.h>
#include <omg/bit_helper.h>
#include <omg/global_box.h>
#include <poincare/context.h>
#include <poincare/exam_mode.h>
#include <stdint.h>

namespace Poincare {

class Expression;

class Preferences final {
 public:
  constexpr static int DefaultNumberOfPrintedSignificantDigits = 10;
  constexpr static int VeryLargeNumberOfSignificantDigits = 7;
  constexpr static int LargeNumberOfSignificantDigits = 6;
  constexpr static int MediumNumberOfSignificantDigits = 5;
  constexpr static int ShortNumberOfSignificantDigits = 4;
  constexpr static int VeryShortNumberOfSignificantDigits = 3;

  enum class EditionMode : uint8_t { Edition2D, Edition1D };
  /* The 'PrintFloatMode' refers to the way to display float 'scientific' or
   * 'auto'. The scientific mode returns float with style -1.2E2 whereas the
   * auto mode tries to return 'natural' float like (0.021) and switches to
   * scientific mode if the float is too small or too big regarding the number
   * of significant digits. */
  enum class PrintFloatMode : uint8_t {
    Decimal = 0,
    Scientific = 1,
    Engineering = 2,
  };
  enum class ComplexFormat : uint8_t { Real = 0, Cartesian = 1, Polar = 2 };
  constexpr static ComplexFormat k_defautComplexFormatIfNotReal =
      ComplexFormat::Cartesian;
  enum class AngleUnit : uint8_t { Radian = 0, Degree = 1, Gradian = 2 };
  enum class UnitFormat : bool { Metric = 0, Imperial = 1 };
  /* The symbol used for combinations and permutations is country-dependent and
   * set in apps but it stored there to be accessible from Poincare */
  enum class CombinatoricSymbols : uint8_t {
    Default = 0,
    LetterWithSubAndSuperscript = 1
  };
  enum class MixedFractions : bool { Disabled = false, Enabled = true };
  enum class LogarithmBasePosition : uint8_t {
    BottomRight = 0,
    TopLeft,
  };
  // This is in Poincare and not in Apps because it's used in Escher
  enum class LogarithmKeyEvent : uint8_t {
    Default,
    WithBaseTen,
    WithEmptyBase
  };
  enum class ParabolaParameter : uint8_t { Default, FocalLength };

  Preferences();
  static OMG::GlobalBox<Preferences> sharedPreferences;

  static Preferences ClonePreferencesWithNewComplexFormat(
      ComplexFormat complexFormat,
      Preferences* preferences = sharedPreferences);

  static ComplexFormat UpdatedComplexFormatWithExpressionInput(
      ComplexFormat complexFormat, const Expression& e, Context* context);

  AngleUnit angleUnit() const { return m_angleUnit; }
  void setAngleUnit(AngleUnit angleUnit) { m_angleUnit = angleUnit; }
  PrintFloatMode displayMode() const { return m_displayMode; }
  void setDisplayMode(PrintFloatMode mode) { m_displayMode = mode; }
  EditionMode editionMode() const { return m_editionMode; }
  void setEditionMode(EditionMode editionMode) { m_editionMode = editionMode; }
  ComplexFormat complexFormat() const { return m_complexFormat; }
  void setComplexFormat(Preferences::ComplexFormat complexFormat) {
    m_complexFormat = complexFormat;
  }
  CombinatoricSymbols combinatoricSymbols() const {
    return m_combinatoricSymbols;
  }
  void setCombinatoricSymbols(CombinatoricSymbols combinatoricSymbols) {
    m_combinatoricSymbols = combinatoricSymbols;
  }
  uint8_t numberOfSignificantDigits() const {
    return m_numberOfSignificantDigits;
  }
  void setNumberOfSignificantDigits(uint8_t numberOfSignificantDigits) {
    m_numberOfSignificantDigits = numberOfSignificantDigits;
  }
  bool mixedFractionsAreEnabled() const { return m_mixedFractionsAreEnabled; }
  void enableMixedFractions(MixedFractions enable) {
    m_mixedFractionsAreEnabled = static_cast<bool>(enable);
  }
  LogarithmBasePosition logarithmBasePosition() const {
    return m_logarithmBasePosition;
  }
  void setLogarithmBasePosition(LogarithmBasePosition position) {
    m_logarithmBasePosition = position;
  }
  LogarithmKeyEvent logarithmKeyEvent() const { return m_logarithmKeyEvent; }
  void setLogarithmKeyEvent(LogarithmKeyEvent logarithmKeyEvent) {
    m_logarithmKeyEvent = logarithmKeyEvent;
  }
  ParabolaParameter parabolaParameter() { return m_parabolaParameter; }
  void setParabolaParameter(ParabolaParameter parameter) {
    m_parabolaParameter = parameter;
  }

  ExamMode examMode() const;
  void setExamMode(ExamMode examMode);

 private:
  AngleUnit m_angleUnit;
  PrintFloatMode m_displayMode;
  EditionMode m_editionMode;
  ComplexFormat m_complexFormat;
  uint8_t m_numberOfSignificantDigits;
  mutable CombinatoricSymbols m_combinatoricSymbols;
  mutable ExamMode m_examMode;
  mutable bool m_mixedFractionsAreEnabled;
  mutable LogarithmBasePosition m_logarithmBasePosition;
  mutable LogarithmKeyEvent m_logarithmKeyEvent;
  mutable ParabolaParameter m_parabolaParameter;
  /* Settings that alter layouts should be tracked by
   * CalculationStore::preferencesMightHaveChanged */
};

}  // namespace Poincare

#endif
