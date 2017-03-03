#ifndef POINCARE_PREFERENCES_H
#define POINCARE_PREFERENCES_H

#include <poincare/expression.h>

namespace Poincare {

class Preferences {
public:
  enum class ComplexFormat {
    Algebric = 0,
    Polar = 1
  };
  Preferences();
  static Preferences * sharedPreferences();
  Expression::AngleUnit angleUnit() const;
  void setAngleUnit(Expression::AngleUnit angleUnit);
  Expression::FloatDisplayMode displayMode() const;
  void setDisplayMode(Expression::FloatDisplayMode FloatDisplayMode);
  ComplexFormat complexFormat() const;
  void setComplexFormat(ComplexFormat complexFormat);
private:
  Expression::AngleUnit m_angleUnit;
  Expression::FloatDisplayMode m_displayMode;
  ComplexFormat m_complexFormat;
};

}

#endif
