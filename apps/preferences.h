#ifndef APPS_PREFERENCES_H
#define APPS_PREFERENCES_H

#include <poincare.h>

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
  enum class Language {
    French = 0,
    English = 1
  };
  Preferences();
  Poincare::Expression::AngleUnit angleUnit() const;
  void setAngleUnit(Poincare::Expression::AngleUnit angleUnit);
  Poincare::Expression::FloatDisplayMode displayMode() const;
  void setDisplayMode(Poincare::Expression::FloatDisplayMode FloatDisplayMode);
  NumberType numberType() const;
  void setNumberType(NumberType numberType);
  ComplexFormat complexFormat() const;
  void setComplexFormat(ComplexFormat complexFormat);
  Language language() const;
  void setLanguage(Language language);
private:
  Poincare::Expression::AngleUnit m_angleUnit;
  Poincare::Expression::FloatDisplayMode m_displayMode;
  NumberType m_numberType;
  ComplexFormat m_complexFormat;
  Language m_language;
};

#endif
