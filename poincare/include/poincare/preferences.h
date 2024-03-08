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

  // Calculation preferences

  enum class AngleUnit : uint8_t {
    Radian = 0,
    Degree,
    Gradian,
    NumberOfAngleUnit,
  };
  /* The 'PrintFloatMode' refers to the way to display float 'scientific' or
   * 'auto'. The scientific mode returns float with style -1.2E2 whereas the
   * auto mode tries to return 'natural' float like (0.021) and switches to
   * scientific mode if the float is too small or too big regarding the number
   * of significant digits. */
  enum class PrintFloatMode : uint8_t {
    Decimal = 0,
    Scientific,
    Engineering,
    NumberOfPrintFloatMode,
  };
  enum class EditionMode : bool {
    Edition2D,
    Edition1D,
  };
  enum class ComplexFormat : uint8_t {
    Real = 0,
    Cartesian,
    Polar,
    NumberOfComplexFormat,
  };
  constexpr static ComplexFormat k_defautComplexFormatIfNotReal =
      ComplexFormat::Cartesian;
  constexpr static size_t k_numberOfBitsForAngleUnit =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<unsigned int>(AngleUnit::NumberOfAngleUnit));
  constexpr static size_t k_numberOfBitsForPrintFloatMode =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<unsigned int>(PrintFloatMode::NumberOfPrintFloatMode));
  constexpr static size_t k_numberOfBitsForComplexFormat =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<unsigned int>(ComplexFormat::NumberOfComplexFormat));

  struct CalculationPreferences {
    /* AngleUnit */ uint8_t m_angleUnit : k_numberOfBitsForAngleUnit;
    /* PrintFloatMode */ uint8_t m_displayMode
        : k_numberOfBitsForPrintFloatMode;
    /* EditionMode */ bool m_editionMode : 1;
    /* ComplexFormat */ uint8_t m_complexFormat
        : k_numberOfBitsForComplexFormat;
    uint8_t m_numberOfSignificantDigits;

    AngleUnit angleUnit() const { return static_cast<AngleUnit>(m_angleUnit); }
    void setAngleUnit(AngleUnit angleUnit) {
      m_angleUnit = static_cast<uint8_t>(angleUnit);
    }
    PrintFloatMode displayMode() const {
      return static_cast<PrintFloatMode>(m_displayMode);
    }
    void setDisplayMode(PrintFloatMode displayMode) {
      m_displayMode = static_cast<uint8_t>(displayMode);
    }
    EditionMode editionMode() const {
      return static_cast<EditionMode>(m_editionMode);
    }
    void setEditionMode(EditionMode editionMode) {
      m_editionMode = static_cast<uint8_t>(editionMode);
    }
    ComplexFormat complexFormat() const {
      return static_cast<ComplexFormat>(m_complexFormat);
    }
    void setComplexFormat(Preferences::ComplexFormat complexFormat) {
      m_complexFormat = static_cast<uint8_t>(complexFormat);
    }
    uint8_t numberOfSignificantDigits() const {
      return m_numberOfSignificantDigits;
    }
    void setNumberOfSignificantDigits(uint8_t numberOfSignificantDigits) {
      m_numberOfSignificantDigits = numberOfSignificantDigits;
    }
  };

  // Other preferences

  enum class UnitFormat : bool { Metric = 0, Imperial = 1 };
  /* The symbol used for combinations and permutations is country-dependent and
   * set in apps but it stored there to be accessible from Poincare */
  enum class CombinatoricSymbols : uint8_t {
    Default = 0,
    LetterWithSubAndSuperscript,
  };
  enum class MixedFractions : bool { Disabled = false, Enabled = true };
  enum class LogarithmBasePosition : uint8_t {
    BottomRight = 0,
    TopLeft,
  };
  // This is in Poincare and not in Apps because it's used in Escher
  enum class LogarithmKeyEvent : uint8_t { Default, WithBaseTen };
  enum class ParabolaParameter : uint8_t { Default, FocalLength };

  Preferences();
  static OMG::GlobalBox<Preferences> sharedPreferences;

  static ComplexFormat UpdatedComplexFormatWithExpressionInput(
      ComplexFormat complexFormat, const Expression& e, Context* context);

  CalculationPreferences calculationPreferences() const {
    return m_calculationPreferences;
  }
  AngleUnit angleUnit() const { return m_calculationPreferences.angleUnit(); }
  void setAngleUnit(AngleUnit angleUnit) {
    m_calculationPreferences.setAngleUnit(angleUnit);
  }
  PrintFloatMode displayMode() const {
    return m_calculationPreferences.displayMode();
  }
  void setDisplayMode(PrintFloatMode displayMode) {
    m_calculationPreferences.setDisplayMode(displayMode);
  }
  EditionMode editionMode() const {
    return m_calculationPreferences.editionMode();
  }
  void setEditionMode(EditionMode editionMode) {
    m_calculationPreferences.setEditionMode(editionMode);
  }
  ComplexFormat complexFormat() const {
    return m_calculationPreferences.complexFormat();
  }
  void setComplexFormat(Preferences::ComplexFormat complexFormat) {
    m_calculationPreferences.setComplexFormat(complexFormat);
  }
  CombinatoricSymbols combinatoricSymbols() const {
    return m_combinatoricSymbols;
  }
  void setCombinatoricSymbols(CombinatoricSymbols combinatoricSymbols) {
    m_combinatoricSymbols = combinatoricSymbols;
  }
  uint8_t numberOfSignificantDigits() const {
    return m_calculationPreferences.numberOfSignificantDigits();
  }
  void setNumberOfSignificantDigits(uint8_t numberOfSignificantDigits) {
    m_calculationPreferences.setNumberOfSignificantDigits(
        numberOfSignificantDigits);
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

  uint32_t mathPreferencesCheckSum() const {
    return (static_cast<uint32_t>(complexFormat()) << 8) +
           static_cast<uint32_t>(angleUnit());
  }

 private:
  CalculationPreferences m_calculationPreferences;
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
