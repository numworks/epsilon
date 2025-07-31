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
class Preferences {
  friend PreferencesTestBuilder;

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
  constexpr static CombinatoricSymbols k_defaultCombinatoricSymbol =
      CombinatoricSymbols::Default;
  constexpr static ParabolaParameter k_defaultParabolaParameter =
      ParabolaParameter::Default;

  class PartialInterface {
   public:
    virtual CombinatoricSymbols combinatoricSymbols() const = 0;
    virtual bool mixedFractionsAreEnabled() const = 0;
    virtual LogarithmBasePosition logarithmBasePosition() const = 0;
    virtual TranslateBuiltins translateBuiltins() const = 0;
    virtual void setTranslateBuiltins(TranslateBuiltins translate) = 0;
    virtual ParabolaParameter parabolaParameter() const = 0;
    bool operator==(const PartialInterface&) const = default;
  };

  class Interface : public PartialInterface {
   public:
    virtual bool forceExamModeReload() const = 0;
    virtual ExamMode examMode() const = 0;
    virtual void setExamMode(ExamMode examMode) = 0;
  };

  static void Init(PartialInterface*);
  static Interface* SharedPreferences();

  static ComplexFormat UpdatedComplexFormatWithExpressionInput(
      ComplexFormat complexFormat, const Internal::Tree* e, Context* context,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceDefinedSymbols);

 private:
  static PartialInterface* s_otherPreferences;

  class __attribute__((packed)) Instance : public Interface {
    friend OMG::GlobalBox<Instance>;
    friend Ion::Storage::FileSystem;

   public:
    constexpr static char k_recordName[] = "pr";
    bool operator==(const Instance&) const = default;

    bool forceExamModeReload() const { return m_forceExamModeReload; }
    ExamMode examMode() const;
    void setExamMode(ExamMode examMode);

    CombinatoricSymbols combinatoricSymbols() const {
      return s_otherPreferences->combinatoricSymbols();
    };
    bool mixedFractionsAreEnabled() const {
      return s_otherPreferences->mixedFractionsAreEnabled();
    };
    LogarithmBasePosition logarithmBasePosition() const {
      return s_otherPreferences->logarithmBasePosition();
    };
    TranslateBuiltins translateBuiltins() const {
      return s_otherPreferences->translateBuiltins();
    };
    void setTranslateBuiltins(TranslateBuiltins translate) {
      s_otherPreferences->setTranslateBuiltins(translate);
    };
    ParabolaParameter parabolaParameter() const {
      return s_otherPreferences->parabolaParameter();
    };

   private:
    constexpr static uint8_t k_version = 1;

    /* Preferences is a singleton, hence the private constructor. The unique
     * instance can be accessed through the Preferences::SharedPreferences()
     * pointer. */
    Instance() = default;

    // TODO: update website
    // TODO 2: merge examMode in GlobalPrefs, update website further
    CODE_GUARD(
        poincare_preferences, 1308047289,  //
        uint8_t m_version = k_version;
        mutable ExamMode m_examMode =
            ExamMode(Ion::ExamMode::Ruleset::Uninitialized);
        /* This flag can only be asserted by writing it via DFU. When set,
         * it will force the reactivation of the exam mode after leaving
         * DFU to synchronize the persisting bytes with the Preferences. */
        bool m_forceExamModeReload = false;)
    static_assert(sizeof(CalculationPreferences) == sizeof(uint16_t));
  };
#if PLATFORM_DEVICE
  static_assert(sizeof(Instance) == 8,
                "Class Preferences::Instance changed size");
#endif
};

#if __EMSCRIPTEN__
/* Preferences live in the Storage which does not enforce alignment, so make
 * sure Emscripten cannot attempt unaligned accesses. */
static_assert(std::alignment_of<Preferences>() == 1);
#endif

}  // namespace Poincare

#endif
