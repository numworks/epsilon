#include <poincare/preferences.h>
#include <poincare/complex.h>

namespace Poincare {

Preferences::Preferences() :
  m_angleUnit(Expression::AngleUnit::Degree),
  m_displayMode(PrintFloat::Mode::Decimal),
  m_editionMode(EditionMode::Edition1D),
  m_complexFormat(Expression::ComplexFormat::Cartesian),
  m_numberOfSignificantDigits(PrintFloat::k_numberOfPrintedSignificantDigits)
{
}

Preferences * Preferences::sharedPreferences() {
  static Preferences preferences;
  return &preferences;
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

Preferences::EditionMode Preferences::editionMode() const {
  return m_editionMode;
}

void Preferences::setEditionMode(Preferences::EditionMode editionMode) {
  m_editionMode = editionMode;
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
