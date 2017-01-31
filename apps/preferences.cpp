#include "preferences.h"

Preferences::Preferences() :
  m_angleUnit(AngleUnit::Degree),
  m_displayMode(Expression::DisplayMode::Auto),
  m_numberType(NumberType::Reel),
  m_complexFormat(ComplexFormat::Cartesian),
  m_language(Language::French)
{
}

Preferences::AngleUnit Preferences::angleUnit() const {
  return m_angleUnit;
}

void Preferences::setAngleUnit(AngleUnit angleUnit) {
  if (angleUnit != m_angleUnit) {
    m_angleUnit = angleUnit;
  }
}

Expression::DisplayMode Preferences::displayMode() const {
  return m_displayMode;
}

void Preferences::setDisplayMode(Expression::DisplayMode displayMode) {
  if (displayMode != m_displayMode) {
    m_displayMode = displayMode;
  }
}

Preferences::NumberType Preferences::numberType() const {
  return m_numberType;
}

void Preferences::setNumberType(NumberType numberType) {
  if (numberType != m_numberType) {
    m_numberType = numberType;
  }
}

Preferences::ComplexFormat Preferences::complexFormat() const {
  return m_complexFormat;
}

void Preferences::setComplexFormat(ComplexFormat complexFormat) {
  if (complexFormat != m_complexFormat) {
    m_complexFormat = complexFormat;
  }
}

Preferences::Language Preferences::language() const {
  return m_language;
}

void Preferences::setLanguage(Language language) {
  if (language != m_language) {
    m_language = language;
  }
}
