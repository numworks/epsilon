#ifndef POINCARE_PREFERENCES_H
#define POINCARE_PREFERENCES_H

#include <poincare/expression.h>

namespace Poincare {

class Preferences {
public:
  enum class NumberType {
    Reel = 0,
    Complex = 1
  };
  enum class ComplexFormat {
    Algebric = 0,
    Polar = 1
  };
  Preferences();
  static Preferences * sharedPreferences();
  Poincare::Expression::AngleUnit angleUnit() const;
  void setAngleUnit(Poincare::Expression::AngleUnit angleUnit);
  Poincare::Expression::FloatDisplayMode displayMode() const;
  void setDisplayMode(Poincare::Expression::FloatDisplayMode FloatDisplayMode);
  NumberType numberType() const;
  void setNumberType(NumberType numberType);
  ComplexFormat complexFormat() const;
  void setComplexFormat(ComplexFormat complexFormat);
private:
  Poincare::Expression::AngleUnit m_angleUnit;
  Poincare::Expression::FloatDisplayMode m_displayMode;
  NumberType m_numberType;
  ComplexFormat m_complexFormat;
};

}

#endif
