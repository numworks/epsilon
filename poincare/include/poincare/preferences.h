#ifndef POINCARE_PREFERENCES_H
#define POINCARE_PREFERENCES_H

#include <assert.h>
#include <omg/bit_helper.h>
#include <omg/code_guard.h>
#include <omg/global_box.h>
#include <poincare/exam_mode.h>
#include <poincare/old/context.h>
#include <poincare/src/expression/context.h>
#include <stdint.h>

// Forward-declarations of friend classes in other namespaces
class PreferencesTestBuilder;
namespace Ion::Storage {
class FileSystem;
}

namespace Poincare {

using ReductionTarget = Internal::ReductionTarget;
using SymbolicComputation = Internal::SymbolicComputation;

/**
 * Preferences live in the Storage, which does not enforce alignment. The
 * packed attribute ensures the compiler will not emit instructions that require
 * the data to be aligned.
 * TODO: Ideally the class should be final, but we need to override it with
 * Apps::MathPreferences and Escher::LayoutPreferences. See comment below. */
class __attribute__((packed)) Preferences {
  friend Ion::Storage::FileSystem;
  friend PreferencesTestBuilder;
  friend OMG::GlobalBox<Poincare::Preferences>;

 public:
  constexpr static int DefaultNumberOfPrintedSignificantDigits = 10;
  constexpr static int VeryLargeNumberOfSignificantDigits = 7;
  constexpr static int LargeNumberOfSignificantDigits = 6;
  constexpr static int MediumNumberOfSignificantDigits = 5;
  constexpr static int ShortNumberOfSignificantDigits = 4;
  constexpr static int VeryShortNumberOfSignificantDigits = 3;

  constexpr static char k_recordName[] = "pr";

  // Calculation preferences

  using AngleUnit = Internal::AngleUnit;
  /* The 'PrintFloatMode' refers to the way to display float 'scientific' or
   * 'auto'. The scientific mode returns float with style -1.2E2 whereas the
   * auto mode tries to return 'natural' float like (0.021) and switches to
   * scientific mode if the float is too small or too big regarding the number
   * of significant digits. */
  enum class PrintFloatMode : uint8_t {
    Decimal = 0,
    Scientific,
    Engineering,
    NModes,
  };
  using ComplexFormat = Internal::ComplexFormat;
  constexpr static ComplexFormat k_defaultComplexFormatIfNotReal =
      ComplexFormat::Cartesian;
  // TODO: C++23: use std::to_underlying instead of static_cast
  constexpr static size_t k_numberOfBitsForAngleUnit =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<uint8_t>(AngleUnit::NUnits));
  constexpr static size_t k_numberOfBitsForPrintFloatMode =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<uint8_t>(PrintFloatMode::NModes));
  constexpr static size_t k_numberOfBitsForComplexFormat =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<uint8_t>(ComplexFormat::NFormats));

  struct CalculationPreferences {
    AngleUnit angleUnit : k_numberOfBitsForAngleUnit;
    PrintFloatMode displayMode : k_numberOfBitsForPrintFloatMode;
    bool dummy : 1;  // Filler to avoid changing the size of the struct
    ComplexFormat complexFormat : k_numberOfBitsForComplexFormat;
    /* Explicitly declare padding bits to avoid uninitalized values. */
    uint8_t padding
        : OMG::BitHelper::numberOfBitsIn<uint8_t>() -
          k_numberOfBitsForAngleUnit - k_numberOfBitsForPrintFloatMode -
          1 - k_numberOfBitsForComplexFormat;
    uint8_t numberOfSignificantDigits;

    bool operator==(const CalculationPreferences&) const = default;
  };

  constexpr static CalculationPreferences k_defaultCalculationPreferences = {
      .angleUnit = AngleUnit::Radian,
      .displayMode = Preferences::PrintFloatMode::Decimal,
      .dummy = false,
      .complexFormat = Preferences::ComplexFormat::Real,
      .numberOfSignificantDigits =
          Preferences::DefaultNumberOfPrintedSignificantDigits};

  // Other preferences
  using UnitFormat = Internal::UnitFormat;
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
  enum class TranslateBuiltins : uint8_t {
    No = 0,
    TranslateToFrench,
    // TODO: support other languages
  };
  enum class ParabolaParameter : uint8_t { Default, FocalLength };

  constexpr static MixedFractions k_defaultMixedFraction =
      MixedFractions::Disabled;

  constexpr static LogarithmBasePosition k_defaultLogarithmBasePosition =
      LogarithmBasePosition::BottomRight;

  static void Init();
  static Preferences* SharedPreferences();

  bool operator==(const Preferences&) const = default;

  static ComplexFormat UpdatedComplexFormatWithExpressionInput(
      ComplexFormat complexFormat, const Internal::Tree* e, Context* context,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceDefinedSymbols);

  CombinatoricSymbols combinatoricSymbols() const {
    return m_combinatoricSymbols;
  }
  void setCombinatoricSymbols(CombinatoricSymbols combinatoricSymbols) {
    m_combinatoricSymbols = combinatoricSymbols;
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
  TranslateBuiltins translateBuiltins() const {
#if POINCARE_TRANSLATE_BUILTINS
    return m_translatedBuiltins;
#else
    return TranslateBuiltins::No;
#endif
  }
  void setTranslateBuiltins(TranslateBuiltins translate) {
#if POINCARE_TRANSLATE_BUILTINS
    m_translatedBuiltins = translate;
#endif
  }
  ParabolaParameter parabolaParameter() const { return m_parabolaParameter; }
  void setParabolaParameter(ParabolaParameter parameter) {
    m_parabolaParameter = parameter;
  }

  bool forceExamModeReload() const { return m_forceExamModeReload; }
  ExamMode examMode() const;
  void setExamMode(ExamMode examMode);

 private:
  // TODO: remove this and associated member
  enum class LogarithmKeyEvent : uint8_t { Default };
  constexpr static uint8_t k_version = 0;

  /* Preferences is a singleton, hence the private constructor. The unique
   * instance can be accessed through the Preferences::SharedPreferences()
   * pointer. */
  Preferences() = default;

  CODE_GUARD(
      poincare_preferences, 2067043470,  //
      uint8_t m_version = k_version;
      CalculationPreferences m_calculationPreferences =
          k_defaultCalculationPreferences;
      mutable ExamMode m_examMode =
          ExamMode(Ion::ExamMode::Ruleset::Uninitialized);
      /* This flag can only be asserted by writing it via DFU. When set,
       * it will force the reactivation of the exam mode after leaving
       * DFU to synchronize the persisting bytes with the Preferences. */
      bool m_forceExamModeReload = false;
      CombinatoricSymbols m_combinatoricSymbols = CombinatoricSymbols::Default;
      bool m_mixedFractionsAreEnabled =
          static_cast<bool>(k_defaultMixedFraction);
      LogarithmBasePosition m_logarithmBasePosition =
          k_defaultLogarithmBasePosition;
      LogarithmKeyEvent m_logarithmKeyEvent = LogarithmKeyEvent::Default;
      ParabolaParameter m_parabolaParameter = ParabolaParameter::Default;)

#if POINCARE_TRANSLATE_BUILTINS
  mutable TranslateBuiltins m_translatedBuiltins;
#endif

  /* Settings that alter layouts should be tracked by
   * CalculationStore::preferencesMightHaveChanged */
};

#if PLATFORM_DEVICE && !POINCARE_TRANSLATE_BUILTINS
static_assert(sizeof(Preferences) == 11, "Class Preferences changed size");
#endif

#if __EMSCRIPTEN__
/* Preferences live in the Storage which does not enforce alignment, so make
 * sure Emscripten cannot attempt unaligned accesses. */
static_assert(std::alignment_of<Preferences>() == 1);
#endif

}  // namespace Poincare

#endif
