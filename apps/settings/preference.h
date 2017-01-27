#ifndef SETTINGS_MODEL_H
#define SETTINGS_MODEL_H

namespace Settings {

class Preference {
public:
  enum class AngleUnit {
    Degree,
    Radian
  };
  enum class DisplayMode {
    Auto,
    Scientific
  };
  enum class NumberType {
    Reel,
    Complex
  };
  enum class ComplexFormat {
    Cartesian,
    Polar
  };
  enum class Language {
    French,
    English
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

}

#endif
