#ifndef POINCARE_PREFERENCES_H
#define POINCARE_PREFERENCES_H

#include <poincare/context.h>
#include <ion/bit_helper.h>
#include <ion/include/ion/persisting_bytes.h>
#include <assert.h>
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

  enum class EditionMode : uint8_t {
    Edition2D,
    Edition1D
  };
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
  enum class ComplexFormat : uint8_t {
    Real = 0,
    Cartesian = 1,
    Polar = 2
  };
  constexpr static ComplexFormat k_defautComplexFormatIfNotReal = ComplexFormat::Cartesian;
  enum class AngleUnit : uint8_t {
    Radian = 0,
    Degree = 1,
    Gradian = 2
  };
  enum class UnitFormat : bool {
    Metric = 0,
    Imperial = 1
  };
  /* The symbol used for combinations and permutations is country-dependent and
   * set in apps but it stored there to be accessible from Poincare */
  enum class CombinatoricSymbols : uint8_t {
    Default = 0,
    LetterWithSubAndSuperscript = 1
  };
  enum class MixedFractions : bool {
    Disabled = false,
    Enabled = true
  };
  enum class ExamMode : uint8_t {
    // ExamMode is encoded on 4 bits in the Persisting Bytes
    Off = 0,
    Standard = 1,
    Dutch = 2,
    IBTest = 3,
    PressToTest = 4,
    Portuguese = 5,
    Undefined = 6, // Undefined must be the last ExamMode.
    Unknown = 0b1111,
  };
  enum class LogarithmBasePosition : uint8_t {
    BottomRight = 0,
    TopLeft,
  };

  constexpr static int k_numberOfExamModes = 6;
  static_assert(static_cast<int>(ExamMode::IBTest) == 3, "Preferences::ExamMode::IBTest != 3 but this value is used in ion/src/device/kernel/drivers/led_update.cpp");
  static_assert(static_cast<int>(ExamMode::PressToTest) == 4, "Preferences::ExamMode::PressToTest != 4 but this value is used in ion/src/device/kernel/drivers/led_update.cpp");
  static_assert(k_numberOfExamModes == static_cast<int>(ExamMode::Undefined), "Preferences::ExamMode::Undefined should be last but the number of exam modes does not match.");
  static_assert(k_numberOfExamModes < 8, "The exam mode should be coded on 4 bits with one bit == 1 if and only if mode is Unknown");
  static_assert(Ion::BitHelper::numberOfBitsToCountUpTo(static_cast<int>(ExamMode::Undefined)) < Ion::BitHelper::numberOfBitsToCountUpTo(static_cast<int>(ExamMode::Unknown)), "There should be one bit that is up only for Unknown");
  /* Params are false if the feature is activated (allowed) and true if
   * forbidden. */
  union PressToTestParams {
    bool operator==(PressToTestParams other) {
      return m_value == other.m_value;
    }
    uint16_t m_value;
    /* Warning: The order of allocation of bit-fields within a unit (high-order
     * to low-order or low-order to high-order) is implementation-defined. The
     * alignment of the addressable storage unit is unspecified. */
    struct {
      bool m_equationSolverIsForbidden: 1;
      bool m_inequalityGraphingIsForbidden: 1;
      bool m_implicitPlotsAreForbidden: 1;
      bool m_statsDiagnosticsAreForbidden: 1;
      bool m_vectorsAreForbidden: 1;
      bool m_basedLogarithmIsForbidden: 1;
      bool m_sumIsForbidden: 1;
      bool m_exactResultsAreForbidden: 1;
      bool m_elementsAppIsForbidden: 1;
    };
  };
  union ExamPersistingBytes {
    ExamPersistingBytes(Ion::PersistingBytes::PersistingBytesInt pb) : m_value(pb) {}
    ExamPersistingBytes(ExamMode mode, PressToTestParams params) : m_examMode(mode), m_params(params.m_value) {}

    ExamMode mode() const { return m_examMode; }
    PressToTestParams params() const { return PressToTestParams({m_params}); }
    uint16_t m_value;
  private:
    struct {
      ExamMode m_examMode: 4;
      uint16_t m_params : 12;
    };
  };
  /* By default, a PressToTestParams has all parameters set to false. */
  constexpr static PressToTestParams k_inactivePressToTest = PressToTestParams({0});
  static_assert(sizeof(ExamPersistingBytes) == sizeof(uint16_t), "ExamPersistingBytes should fit in two bytes");

  Preferences();
  static Preferences * sharedPreferences();

  static Preferences ClonePreferencesWithNewComplexFormat(ComplexFormat complexFormat, Preferences * preferences = sharedPreferences());

  static ComplexFormat UpdatedComplexFormatWithExpressionInput(ComplexFormat complexFormat, const Expression & e, Context * context);

  AngleUnit angleUnit() const { return m_angleUnit; }
  void setAngleUnit(AngleUnit angleUnit) { m_angleUnit = angleUnit; }
  PrintFloatMode displayMode() const { return m_displayMode; }
  void setDisplayMode(PrintFloatMode mode) { m_displayMode = mode; }
  EditionMode editionMode() const { return m_editionMode; }
  void setEditionMode(EditionMode editionMode) { m_editionMode = editionMode; }
  ComplexFormat complexFormat() const { return m_complexFormat; }
  void setComplexFormat(Preferences::ComplexFormat complexFormat) { m_complexFormat = complexFormat; }
  CombinatoricSymbols combinatoricSymbols() const { return m_combinatoricSymbols; }
  void setCombinatoricSymbols(CombinatoricSymbols combinatoricSymbols) { m_combinatoricSymbols = combinatoricSymbols; }
  uint8_t numberOfSignificantDigits() const { return m_numberOfSignificantDigits; }
  void setNumberOfSignificantDigits(uint8_t numberOfSignificantDigits) { m_numberOfSignificantDigits = numberOfSignificantDigits; }
  bool mixedFractionsAreEnabled() const { return m_mixedFractionsAreEnabled; }
  void enableMixedFractions(MixedFractions enable) { m_mixedFractionsAreEnabled = static_cast<bool>(enable); }
  LogarithmBasePosition logarithmBasePosition() const { return m_logarithmBasePosition; }
  void setLogarithmBasePosition(LogarithmBasePosition position) { m_logarithmBasePosition = position; }

  static_assert((int8_t)Preferences::ExamMode::Off == 0, "Preferences::isInExamMode() relies on exam modes order");
  bool isInExamMode() const { return (int8_t)examMode() > 0; }
  ExamMode examMode() const;
  PressToTestParams pressToTestParams() const;
  void setExamMode(ExamMode examMode, PressToTestParams pressToTestParams = k_inactivePressToTest);
  bool elementsAppIsForbidden() const;
  bool equationSolverIsForbidden() const;
  bool inequalityGraphingIsForbidden() const;
  bool implicitPlotsAreForbidden() const;
  bool statsDiagnosticsAreForbidden() const;
  bool vectorProductsAreForbidden() const;
  bool vectorNormIsForbidden() const;
  bool basedLogarithmIsForbidden() const;
  bool sumIsForbidden() const;
  bool exactResultsAreForbidden() const;
private:
  constexpr static int k_pressToTestParamsPersistingByteIndex = 0;
  constexpr static int k_examModePersistingByteIndex = 1;
  static_assert(k_examModePersistingByteIndex == 1, "k_examModePersistingByteIndex != 1 but this value is used in ion/src/device/kernel/drivers/led_update.cpp");

  void updateExamModeFromPersistingBytesIfNeeded() const;

  AngleUnit m_angleUnit;
  PrintFloatMode m_displayMode;
  EditionMode m_editionMode;
  ComplexFormat m_complexFormat;
  uint8_t m_numberOfSignificantDigits;
  mutable CombinatoricSymbols m_combinatoricSymbols;
  mutable ExamMode m_examMode;
  mutable PressToTestParams m_pressToTestParams;
  mutable bool m_mixedFractionsAreEnabled;
  mutable LogarithmBasePosition m_logarithmBasePosition;
  /* Settings that alter layouts should be tracked by
   * CalculationStore::preferencesMightHaveChanged */
};

}

#endif
