#ifndef POINCARE_PREFERENCES_H
#define POINCARE_PREFERENCES_H

namespace Poincare {

class Preferences final {
public:
  enum class EditionMode {
    Edition2D,
    Edition1D
  };
  /* The 'PrintFlotMode' refers to the way to display float 'scientific' or
   * 'auto'. The scientific mode returns float with style -1.2E2 whereas the
   * auto mode tries to return 'natural' float like (0.021) and switches to
   * scientific mode if the float is too small or too big regarding the number
   * of significant digits. */
  enum class PrintFloatMode {
    Decimal = 0,
    Scientific = 1,
  };
  enum class ComplexFormat {
    Cartesian = 0,
    Polar = 1
  };
  enum class AngleUnit {
    Degree = 0,
    Radian = 1
  };
  Preferences();
  static Preferences * sharedPreferences();
  AngleUnit angleUnit() const;
  void setAngleUnit(AngleUnit angleUnit);
  PrintFloatMode displayMode() const;
  void setDisplayMode(PrintFloatMode mode);
  EditionMode editionMode() const;
  void setEditionMode(EditionMode editionMode);
  ComplexFormat complexFormat() const;
  void setComplexFormat(Preferences::ComplexFormat complexFormat);
  char numberOfSignificantDigits() const;
  void setNumberOfSignificantDigits(char numberOfSignificantDigits);
private:
  AngleUnit m_angleUnit;
  PrintFloatMode m_displayMode;
  EditionMode m_editionMode;
  ComplexFormat m_complexFormat;
  char m_numberOfSignificantDigits;
};

}

#endif
