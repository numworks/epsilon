#ifndef POINCARE_PREFERENCES_H
#define POINCARE_PREFERENCES_H

#include <stdint.h>

namespace Poincare {

class Preferences final {
public:
  constexpr static int DefaultNumberOfPrintedSignificantDigits = 10;
  constexpr static int LargeNumberOfSignificantDigits = 7;
  constexpr static int MediumNumberOfSignificantDigits = 5;
  constexpr static int ShortNumberOfSignificantDigits = 4;
  constexpr static int VeryShortNumberOfSignificantDigits = 3;

  enum class EditionMode {
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
  enum class AngleUnit : uint8_t {
    Radian = 0,
    Degree = 1,
    Gradian = 2
  };
  enum class UnitFormat : uint8_t {
    Metric = 0,
    Imperial = 1
  };
  enum class ExamMode : int8_t {
    Unknown = -1,
    Off = 0,
    Standard = 1,
    Dutch = 2,
    PressToTest = 3
  };
  /* By default, a 0 PressToTestParams has all parameters set to false. Params
   * are false if the feature is activated (allowed) and true if forbidden. */
  typedef union {
    uint8_t m_value;
    struct {
      bool m_isUnknown: 1;
      bool m_equationSolverIsForbidden: 1;
      bool m_inequalityGraphingIsForbidden: 1;
      bool m_implicitPlotsAreForbidden: 1;
      bool m_statsDiagnosticsAreForbidden: 1;
      bool m_vectorsAreForbidden: 1;
      bool m_basedLogarithmIsForbidden: 1;
      bool m_sumIsForbidden: 1;
    };
  } PressToTestParams;
  static_assert(sizeof(PressToTestParams) == sizeof(uint8_t), "PressToTestParams can have 8 params at most");

  Preferences();
  static Preferences * sharedPreferences();
  AngleUnit angleUnit() const { return m_angleUnit; }
  void setAngleUnit(AngleUnit angleUnit) { m_angleUnit = angleUnit; }
  PrintFloatMode displayMode() const { return m_displayMode; }
  void setDisplayMode(PrintFloatMode mode) { m_displayMode = mode; }
  EditionMode editionMode() const { return m_editionMode; }
  void setEditionMode(EditionMode editionMode) { m_editionMode = editionMode; }
  ComplexFormat complexFormat() const { return m_complexFormat; }
  void setComplexFormat(Preferences::ComplexFormat complexFormat) { m_complexFormat = complexFormat; }
  uint8_t numberOfSignificantDigits() const { return m_numberOfSignificantDigits; }
  void setNumberOfSignificantDigits(uint8_t numberOfSignificantDigits) { m_numberOfSignificantDigits = numberOfSignificantDigits; }

  static_assert((int8_t)Preferences::ExamMode::Off == 0 && (int8_t)Preferences::ExamMode::Unknown < 0, "Preferences::isInExamMode() relies on exam modes order");
  bool isInExamMode() const { return (int8_t)examMode() > 0; }
  ExamMode examMode() const;
  PressToTestParams pressToTestParams() const;
  void setExamMode(ExamMode examMode);
  void setPressToTestParams(PressToTestParams pressToTestParams);
  bool equationSolverIsForbidden() const;
  bool inequalityGraphingIsForbidden() const;
  bool implicitPlotsAreForbidden() const;
  bool statsDiagnosticsAreForbidden() const;
  bool vectorsAreForbidden() const;
  bool basedLogarithmIsForbidden() const;
  bool sumIsForbidden() const;
private:
  static constexpr int k_examModePersistingByteIndex = 0;
  static constexpr int k_pressToTestParamsPersistingByteIndex = 1;

  AngleUnit m_angleUnit;
  PrintFloatMode m_displayMode;
  EditionMode m_editionMode;
  ComplexFormat m_complexFormat;
  uint8_t m_numberOfSignificantDigits;
  mutable ExamMode m_examMode;
  mutable PressToTestParams m_pressToTestParams;
};

}

#endif
