#include <poincare/preferences.h>

namespace Poincare {

static Preferences s_preferences;

Preferences::Preferences() :
  m_angleUnit(Expression::AngleUnit::Degree),
  m_displayMode(Expression::FloatDisplayMode::Decimal),
  m_numberType(NumberType::Reel),
  m_complexFormat(ComplexFormat::Algebric)
{
}

Preferences * Preferences::sharedPreferences() {
  return &s_preferences;
}

Expression::AngleUnit Preferences::angleUnit() const {
  return m_angleUnit;
}

void Preferences::setAngleUnit(Expression::AngleUnit angleUnit) {
  if (angleUnit != m_angleUnit) {
    m_angleUnit = angleUnit;
  }
}

Expression::FloatDisplayMode Preferences::displayMode() const {
  return m_displayMode;
}

void Preferences::setDisplayMode(Expression::FloatDisplayMode FloatDisplayMode) {
  if (FloatDisplayMode != m_displayMode) {
    m_displayMode = FloatDisplayMode;
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

}
