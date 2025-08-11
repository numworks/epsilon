#ifndef POINCARE_PREFERENCES_H
#define POINCARE_PREFERENCES_H

#include <assert.h>
#include <omg/bit_helper.h>
#include <omg/code_guard.h>
#include <omg/global_box.h>
#include <poincare/exam_mode.h>
#include <poincare/src/expression/context.h>
#include <stdint.h>

#include "context.h"

namespace Poincare {

using ReductionTarget = Internal::ReductionTarget;
using SymbolicComputation = Internal::SymbolicComputation;

/* [Preferences] is a singleton accessible via the [SharedPreferences] instance.
 * The object gives access to an implementation of [Preferences::Interface].
 * The class also defines (or uses) structs and enums relative to Poincare
 * settings.
 */
class Preferences {
 public:
  constexpr static int DefaultNumberOfPrintedSignificantDigits = 10;
  constexpr static int VeryLargeNumberOfSignificantDigits = 7;
  constexpr static int LargeNumberOfSignificantDigits = 6;
  constexpr static int MediumNumberOfSignificantDigits = 5;
  constexpr static int ShortNumberOfSignificantDigits = 4;
  constexpr static int VeryShortNumberOfSignificantDigits = 3;

  // --- Calculation preferences ---

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

  // NOTE: Add a CODE_GUARD here if the prefs are used in the website
  struct CalculationPreferences {
    uint8_t numberOfSignificantDigits;
    AngleUnit angleUnit : k_numberOfBitsForAngleUnit;
    PrintFloatMode displayMode : k_numberOfBitsForPrintFloatMode;
    ComplexFormat complexFormat : k_numberOfBitsForComplexFormat;
    /* Explicitly declare padding bits to avoid uninitalized values. */
    uint8_t padding
        : OMG::BitHelper::numberOfBitsIn<uint8_t>() -
          k_numberOfBitsForAngleUnit - k_numberOfBitsForPrintFloatMode -
          k_numberOfBitsForComplexFormat;
    bool operator==(const CalculationPreferences&) const = default;
  };

  constexpr static CalculationPreferences k_defaultCalculationPreferences = {
      .numberOfSignificantDigits =
          Preferences::DefaultNumberOfPrintedSignificantDigits,
      .angleUnit = AngleUnit::Radian,
      .displayMode = Preferences::PrintFloatMode::Decimal,
      .complexFormat = Preferences::ComplexFormat::Real,
      .padding = 0,
  };

  // --- Country-dependent preferences ---

  using UnitFormat = Internal::UnitFormat;

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

  /* These country-dependent preferences are needed inside Poincare.
   * This [Interface] allows the storage of those preferences to be outside
   * Poincare while maintaining Poincare's independence */
  class Interface {
   public:
    virtual CombinatoricSymbols combinatoricSymbols() const = 0;
    virtual bool mixedFractionsAreEnabled() const = 0;
    virtual LogarithmBasePosition logarithmBasePosition() const = 0;
    virtual TranslateBuiltins translateBuiltins() const = 0;
    // TODO: setTranslateBuiltins should only be accessible in Scandium
    virtual void setTranslateBuiltins(TranslateBuiltins translate) = 0;
    virtual ParabolaParameter parabolaParameter() const = 0;
    bool operator==(const Interface&) const = default;
  };

  static void Init(Interface*);
  Interface* operator->() const { return s_preferences; }

  static const Preferences PreferencesInstance;

  // TODO: this has nothing to do here
  static ComplexFormat UpdatedComplexFormatWithExpressionInput(
      ComplexFormat complexFormat, const Internal::Tree* e, Context* context,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceDefinedSymbols);

 private:
  Preferences() = default;
  static Interface* s_preferences;
};
static_assert(sizeof(Preferences) == 1,
              "Preferences should not contain anything");

inline constexpr const Preferences& SharedPreferences =
    Preferences::PreferencesInstance;

#if __EMSCRIPTEN__
/* Preferences live in the Storage which does not enforce alignment, so make
 * sure Emscripten cannot attempt unaligned accesses. */
static_assert(std::alignment_of<Preferences>() == 1);
#endif

}  // namespace Poincare

#endif
