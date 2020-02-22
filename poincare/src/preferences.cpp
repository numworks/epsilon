#include <poincare/preferences.h>
#include <poincare/print_float.h>

namespace Poincare {

constexpr int Preferences::LargeNumberOfSignificantDigits;
constexpr int Preferences::MediumNumberOfSignificantDigits;
constexpr int Preferences::ShortNumberOfSignificantDigits;

Preferences::Preferences() :
  m_angleUnit(AngleUnit::Degree),
  m_displayMode(Preferences::PrintFloatMode::Decimal),
  m_editionMode(EditionMode::Edition2D),
  m_complexFormat(Preferences::ComplexFormat::Real),
  m_numberOfSignificantDigits(PrintFloat::k_numberOfPrintedSignificantDigits)
{}

Preferences * Preferences::sharedPreferences() {
  static Preferences preferences;
  return &preferences;
}

}
