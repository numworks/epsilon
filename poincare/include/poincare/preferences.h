#ifndef POINCARE_PREFERENCES_H
#define POINCARE_PREFERENCES_H

#include <stdint.h>
#include <kandinsky.h>

namespace Poincare {

class Preferences final {
public:
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
    Degree = 0,
    Radian = 1,
    Gradian = 2
  };
  enum class SymbolMultiplication : uint8_t {
    Cross = 0,
    MiddleDot = 1,
    Star = 2,
    Auto = 3
  };
  enum class SymbolFunction : uint8_t {
    Default = 0,
    ArgDefault = 1,
    Arg = 2   
  };
  enum class PythonFont : uint8_t {
    Large = 0,
    Small = 1
  };
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
  SymbolMultiplication symbolOfMultiplication() const { return m_symbolMultiplication; }
  void setSymbolMultiplication(SymbolMultiplication symbolOfMultiplication) { m_symbolMultiplication = symbolOfMultiplication; }
  SymbolFunction symbolOfFunction() const { return m_symbolFunction; }
  void setSymbolOfFunction(SymbolFunction symbolOfFunction) { m_symbolFunction = symbolOfFunction; }
  PythonFont pythonFont() const { return m_pythonFont; }
  void setPythonFont(PythonFont pythonFont) { m_pythonFont = pythonFont; }
  const KDFont * KDPythonFont() const { return (m_pythonFont == PythonFont::Small) ? KDFont::SmallFont : KDFont::LargeFont; }
private:
  AngleUnit m_angleUnit;
  PrintFloatMode m_displayMode;
  EditionMode m_editionMode;
  ComplexFormat m_complexFormat;
  uint8_t m_numberOfSignificantDigits;
  SymbolMultiplication m_symbolMultiplication;
  SymbolFunction m_symbolFunction;
  PythonFont m_pythonFont;
};

}

#endif
