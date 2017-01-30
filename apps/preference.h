#ifndef APPS_PREFERENCE_H
#define APPS_PREFERENCE_H

class Preference {
public:
  enum class AngleUnit {
    Degree = 0,
    Radian = 1
  };
  enum class DisplayMode {
    Auto = 0,
    Scientific = 1
  };
  enum class NumberType {
    Reel = 0,
    Complex = 1
  };
  enum class ComplexFormat {
    Cartesian = 0,
    Polar = 1
  };
  enum class Language {
    French = 0,
    English = 1
  };
  Preference();
  AngleUnit angleUnit() const;
  void setAngleUnit(AngleUnit angleUnit);
  DisplayMode displayMode() const;
  void setDisplayMode(DisplayMode displayMode);
  NumberType numberType() const;
  void setNumberType(NumberType numberType);
  ComplexFormat complexFormat() const;
  void setComplexFormat(ComplexFormat complexFormat);
  Language language() const;
  void setLanguage(Language language);
private:
  AngleUnit m_angleUnit;
  DisplayMode m_displayMode;
  NumberType m_numberType;
  ComplexFormat m_complexFormat;
  Language m_language;
};

#endif
