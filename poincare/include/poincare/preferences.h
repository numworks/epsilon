#ifndef POINCARE_PREFERENCES_H
#define POINCARE_PREFERENCES_H

#include <assert.h>
#include <omg/bit_helper.h>
#include <omg/code_guard.h>
#include <omg/global_box.h>
#include <poincare/context.h>
#include <poincare/exam_mode.h>
#include <poincare_junior/src/expression/context.h>
#include <stdint.h>

namespace Poincare {

class JuniorExpression;

/* Preferences live in the Storage, which does not enforce alignment. The packed
 * attribute ensures the compiler will not emit instructions that require the
 * data to be aligned. */
class __attribute__((packed)) Preferences final {
 public:
  constexpr static int DefaultNumberOfPrintedSignificantDigits = 10;
  constexpr static int VeryLargeNumberOfSignificantDigits = 7;
  constexpr static int LargeNumberOfSignificantDigits = 6;
  constexpr static int MediumNumberOfSignificantDigits = 5;
  constexpr static int ShortNumberOfSignificantDigits = 4;
  constexpr static int VeryShortNumberOfSignificantDigits = 3;

  constexpr static char k_recordName[] = "pr";

  // Calculation preferences

  using AngleUnit = PoincareJ::AngleUnit;
  /* The 'PrintFloatMode' refers to the way to display float 'scientific' or
   * 'auto'. The scientific mode returns float with style -1.2E2 whereas the
   * auto mode tries to return 'natural' float like (0.021) and switches to
   * scientific mode if the float is too small or too big regarding the number
   * of significant digits. */
  enum class PrintFloatMode : uint8_t {
    Decimal = 0,
    Scientific,
    Engineering,
    LastPrintFloatMode = Engineering,
  };
  enum class EditionMode : bool {
    Edition2D,
    Edition1D,
  };
  using ComplexFormat = PoincareJ::ComplexFormat;
  constexpr static ComplexFormat k_defautComplexFormatIfNotReal =
      ComplexFormat::Cartesian;
  constexpr static size_t k_numberOfBitsForAngleUnit =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<unsigned int>(AngleUnit::LastAngleUnit) + 1);
  constexpr static size_t k_numberOfBitsForPrintFloatMode =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<unsigned int>(PrintFloatMode::LastPrintFloatMode) + 1);
  constexpr static size_t k_numberOfBitsForComplexFormat =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<unsigned int>(ComplexFormat::LastComplexFormat) + 1);

  struct CalculationPreferences {
    AngleUnit angleUnit : k_numberOfBitsForAngleUnit;
    PrintFloatMode displayMode : k_numberOfBitsForPrintFloatMode;
    EditionMode editionMode : 1;
    ComplexFormat complexFormat : k_numberOfBitsForComplexFormat;
    /* Explicitly declare padding bits to avoid uninitalized values. */
    uint8_t padding
        : OMG::BitHelper::numberOfBitsIn<uint8_t>() -
          k_numberOfBitsForAngleUnit - k_numberOfBitsForPrintFloatMode -
          1 - k_numberOfBitsForComplexFormat;
    uint8_t numberOfSignificantDigits;
  };

  // Other preferences
  using UnitFormat = PoincareJ::UnitFormat;
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
  static void Init();
  static Preferences* SharedPreferences();

  static ComplexFormat UpdatedComplexFormatWithExpressionInput(
      ComplexFormat complexFormat, const JuniorExpression& e, Context* context);

  CalculationPreferences calculationPreferences() const {
    return m_calculationPreferences;
  }
  AngleUnit angleUnit() const { return m_calculationPreferences.angleUnit; }
  void setAngleUnit(AngleUnit angleUnit) {
    m_calculationPreferences.angleUnit = angleUnit;
  }
  PrintFloatMode displayMode() const {
    return m_calculationPreferences.displayMode;
  }
  void setDisplayMode(PrintFloatMode displayMode) {
    m_calculationPreferences.displayMode = displayMode;
  }
  EditionMode editionMode() const {
    return m_calculationPreferences.editionMode;
  }
  void setEditionMode(EditionMode editionMode) {
    m_calculationPreferences.editionMode = editionMode;
  }
  ComplexFormat complexFormat() const {
    return m_calculationPreferences.complexFormat;
  }
  void setComplexFormat(Preferences::ComplexFormat complexFormat) {
    m_calculationPreferences.complexFormat = complexFormat;
  }
  CombinatoricSymbols combinatoricSymbols() const {
    return m_combinatoricSymbols;
  }
  void setCombinatoricSymbols(CombinatoricSymbols combinatoricSymbols) {
    m_combinatoricSymbols = combinatoricSymbols;
  }
  uint8_t numberOfSignificantDigits() const {
    return m_calculationPreferences.numberOfSignificantDigits;
  }
  void setNumberOfSignificantDigits(uint8_t numberOfSignificantDigits) {
    m_calculationPreferences.numberOfSignificantDigits =
        numberOfSignificantDigits;
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

  bool forceExamModeReload() const { return m_forceExamModeReload; }
  ExamMode examMode() const;
  void setExamMode(ExamMode examMode);

  uint32_t mathPreferencesCheckSum() const {
    return (static_cast<uint32_t>(complexFormat()) << 8) +
           static_cast<uint32_t>(angleUnit());
  }

 private:
  constexpr static uint8_t k_version = 0;

  CODE_GUARD(poincare_preferences, 524861357,  //
             uint8_t m_version;
             CalculationPreferences m_calculationPreferences;
             mutable ExamMode m_examMode;
             /* This flag can only be asserted by writing it via DFU. When set,
              * it will force the reactivation of the exam mode after leaving
              * DFU to synchronize the persisting bytes with the Preferences. */
             bool m_forceExamModeReload;
             mutable CombinatoricSymbols m_combinatoricSymbols;
             mutable bool m_mixedFractionsAreEnabled;
             mutable LogarithmBasePosition m_logarithmBasePosition;
             mutable LogarithmKeyEvent m_logarithmKeyEvent;
             mutable ParabolaParameter m_parabolaParameter;)

  /* Settings that alter layouts should be tracked by
   * CalculationStore::preferencesMightHaveChanged */
};

#if PLATFORM_DEVICE
static_assert(sizeof(Preferences) == 11, "Class Preferences changed size");
#endif

#if __EMSCRIPTEN__
/* Preferences live in the Storage which does not enforce alignment, so make
 * sure Emscripten cannot attempt unaligned accesses. */
static_assert(std::alignment_of<Preferences>() == 1);
#endif

}  // namespace Poincare

#endif
