#include <poincare/preferences.h>
#include <poincare/complex.h>

namespace Poincare {

static Preferences s_preferences;

Preferences::Preferences() :
  m_angleUnit(Expression::AngleUnit::Degree),
  m_displayMode(PrintFloat::Mode::Decimal),
  m_complexFormat(Expression::ComplexFormat::Cartesian),
  m_numberOfSignificantDigits(PrintFloat::k_numberOfPrintedSignificantDigits)
{
}

Preferences * Preferences::sharedPreferences() {
  return &s_preferences;
}

Expression::AngleUnit Preferences::angleUnit() const {
  return m_angleUnit;
}

void Preferences::setAngleUnit(Expression::AngleUnit angleUnit) {
  m_angleUnit = angleUnit;
}

PrintFloat::Mode Preferences::displayMode() const {
  return m_displayMode;
}

void Preferences::setDisplayMode(PrintFloat::Mode mode) {
  m_displayMode = mode;
}

Expression::ComplexFormat Preferences::complexFormat() const {
  return m_complexFormat;
}

void Preferences::setComplexFormat(Expression::ComplexFormat complexFormat) {
  m_complexFormat = complexFormat;
}

char Preferences::numberOfSignificantDigits() const {
  return m_numberOfSignificantDigits;
}

void Preferences::setNumberOfSignificantDigits(char numberOfSignificantDigits) {
  m_numberOfSignificantDigits = numberOfSignificantDigits;
}

}
