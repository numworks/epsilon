#include "preference.h"

Preference::Preference() :
  m_angleUnit(AngleUnit::Degree),
  m_displayMode(DisplayMode::Auto),
  m_numberType(NumberType::Reel),
  m_complexFormat(ComplexFormat::Cartesian),
  m_language(Language::French)
{
}

Preference::AngleUnit Preference::angleUnit() const {
  return m_angleUnit;
}

void Preference::setAngleUnit(AngleUnit angleUnit) {
  if (angleUnit != m_angleUnit) {
    m_angleUnit = angleUnit;
  }
}

Preference::DisplayMode Preference::displayMode() const {
  return m_displayMode;
}

void Preference::setDisplayMode(DisplayMode displayMode) {
  if (displayMode != m_displayMode) {
    m_displayMode = displayMode;
  }
}

Preference::NumberType Preference::numberType() const {
  return m_numberType;
}

void Preference::setNumberType(NumberType numberType) {
  if (numberType != m_numberType) {
    m_numberType = numberType;
  }
}

Preference::ComplexFormat Preference::complexFormat() const {
  return m_complexFormat;
}

void Preference::setComplexFormat(ComplexFormat complexFormat) {
  if (complexFormat != m_complexFormat) {
    m_complexFormat = complexFormat;
  }
}

Preference::Language Preference::language() const {
  return m_language;
}

void Preference::setLanguage(Language language) {
  if (language != m_language) {
    m_language = language;
  }
}
