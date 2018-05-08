#ifndef POINCARE_PREFERENCES_H
#define POINCARE_PREFERENCES_H

#include <poincare/expression.h>

namespace Poincare {

class Preferences {
public:
  enum class EditionMode {
    Edition1D,
    Edition2D
  };
  Preferences();
  static Preferences * sharedPreferences();
  Expression::AngleUnit angleUnit() const;
  void setAngleUnit(Expression::AngleUnit angleUnit);
  PrintFloat::Mode displayMode() const;
  void setDisplayMode(PrintFloat::Mode mode);
  EditionMode editionMode() const;
  void setEditionMode(EditionMode editionMode);
  Expression::ComplexFormat complexFormat() const;
  void setComplexFormat(Expression::ComplexFormat complexFormat);
  char numberOfSignificantDigits() const;
  void setNumberOfSignificantDigits(char numberOfSignificantDigits);
private:
  Expression::AngleUnit m_angleUnit;
  PrintFloat::Mode m_displayMode;
  EditionMode m_editionMode;
  Expression::ComplexFormat m_complexFormat;
  char m_numberOfSignificantDigits;
};

}

#endif
