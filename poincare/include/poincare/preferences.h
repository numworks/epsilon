#ifndef POINCARE_PREFERENCES_H
#define POINCARE_PREFERENCES_H

#include <poincare/expression.h>
#include <escher/palette.h>

namespace Poincare {

class Preferences {
public:
  Preferences();
  static Preferences * sharedPreferences();
  Expression::AngleUnit angleUnit() const;
  void setAngleUnit(Expression::AngleUnit angleUnit);
  Expression::FloatDisplayMode displayMode() const;
  void setDisplayMode(Expression::FloatDisplayMode FloatDisplayMode);
  Expression::ComplexFormat complexFormat() const;
  void setComplexFormat(Expression::ComplexFormat complexFormat);
  Expression::Theme theme() const;
  KDColor themeDarkColor() const;
  KDColor themeLightColor() const;
  void setTheme(Expression::Theme theme);
private:
  Expression::AngleUnit m_angleUnit;
  Expression::FloatDisplayMode m_displayMode;
  Expression::ComplexFormat m_complexFormat;
  Expression::Theme m_theme;
};

}

#endif
