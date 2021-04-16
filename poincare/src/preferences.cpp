#include <poincare/preferences.h>

namespace Poincare {

constexpr int Preferences::DefaultNumberOfPrintedSignificantDigits;
constexpr int Preferences::LargeNumberOfSignificantDigits;
constexpr int Preferences::MediumNumberOfSignificantDigits;
constexpr int Preferences::ShortNumberOfSignificantDigits;
constexpr int Preferences::VeryShortNumberOfSignificantDigits;

Preferences::Preferences() :
  m_angleUnit(AngleUnit::Degree),
  m_displayMode(Preferences::PrintFloatMode::Decimal),
  m_editionMode(EditionMode::Edition2D),
  m_complexFormat(Preferences::ComplexFormat::Real),
  m_numberOfSignificantDigits(Preferences::DefaultNumberOfPrintedSignificantDigits)
{}

Preferences * Preferences::sharedPreferences() {
  static Preferences preferences;
  return &preferences;
}

}
