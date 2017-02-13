#ifndef APPS_PREFERENCES_H
#define APPS_PREFERENCES_H

#include <poincare.h>

class Preferences {
public:
  enum class AngleUnit {
    Degree = 0,
    Radian = 1
  };
  enum class NumberType {
    Reel = 0,
    Complex = 1
  };
  enum class ComplexFormat {
    Algebric = 0,
    Polar = 1
  };
  enum class Language {
    French = 0,
    English = 1
  };
  Preferences();
  AngleUnit angleUnit() const;
  void setAngleUnit(AngleUnit angleUnit);
  Expression::DisplayMode displayMode() const;
  void setDisplayMode(Expression::DisplayMode displayMode);
  NumberType numberType() const;
  void setNumberType(NumberType numberType);
  ComplexFormat complexFormat() const;
  void setComplexFormat(ComplexFormat complexFormat);
  Language language() const;
  void setLanguage(Language language);
private:
  AngleUnit m_angleUnit;
  Expression::DisplayMode m_displayMode;
  NumberType m_numberType;
  ComplexFormat m_complexFormat;
  Language m_language;
};

#endif
