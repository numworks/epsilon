#include <poincare/preferences.h>
#include <poincare/print_float.h>

namespace Poincare {

Preferences::Preferences() :
  m_angleUnit(AngleUnit::Degree),
  m_displayMode(Preferences::PrintFloatMode::Decimal),
  m_editionMode(EditionMode::Edition2D),
  m_complexFormat(Preferences::ComplexFormat::Cartesian),
  m_numberOfSignificantDigits(PrintFloat::k_numberOfPrintedSignificantDigits)
{
}

Preferences * Preferences::sharedPreferences() {
  static Preferences preferences;
  return &preferences;
}

Preferences::AngleUnit Preferences::angleUnit() const {
  return m_angleUnit;
}

void Preferences::setAngleUnit(AngleUnit angleUnit) {
  m_angleUnit = angleUnit;
}

Preferences::PrintFloatMode Preferences::displayMode() const {
  return m_displayMode;
}

void Preferences::setDisplayMode(Preferences::PrintFloatMode mode) {
  m_displayMode = mode;
}

Preferences::EditionMode Preferences::editionMode() const {
  return m_editionMode;
}

void Preferences::setEditionMode(Preferences::EditionMode editionMode) {
  m_editionMode = editionMode;
}

Preferences::ComplexFormat Preferences::complexFormat() const {
  return m_complexFormat;
}

void Preferences::setComplexFormat(ComplexFormat complexFormat) {
  m_complexFormat = complexFormat;
}

char Preferences::numberOfSignificantDigits() const {
  return m_numberOfSignificantDigits;
}

void Preferences::setNumberOfSignificantDigits(char numberOfSignificantDigits) {
  m_numberOfSignificantDigits = numberOfSignificantDigits;
}

}
