#include <poincare/preferences.h>
#include <escher/palette.h>

namespace Poincare {

static Preferences s_preferences;

Preferences::Preferences() :
  m_angleUnit(Expression::AngleUnit::Degree),
  m_displayMode(Expression::FloatDisplayMode::Decimal),
  m_complexFormat(Expression::ComplexFormat::Cartesian),
  m_theme(Expression::Theme::Yellow)
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

Expression::ComplexFormat Preferences::complexFormat() const {
  return m_complexFormat;
}

void Preferences::setComplexFormat(Expression::ComplexFormat complexFormat) {
  if (complexFormat != m_complexFormat) {
    m_complexFormat = complexFormat;
  }
}

Expression::Theme Preferences::theme() const {
  return m_theme;
}

KDColor Preferences::themeDarkColor() const {
  switch(m_theme) {
    default:
    case Expression::Theme::Yellow:
      return Palette::YellowDark;
    case Expression::Theme::Blue:
      return Palette::BlueDark;
    case Expression::Theme::Green:
      return Palette::GreenDark;
    case Expression::Theme::Red:
      return Palette::RedDark;
  }
}

KDColor Preferences::themeLightColor() const {
  switch(m_theme) {
    default:
    case Expression::Theme::Yellow:
      return Palette::YellowLight;
    case Expression::Theme::Blue:
      return Palette::BlueLight;
    case Expression::Theme::Green:
      return Palette::GreenLight;
    case Expression::Theme::Red:
      return Palette::RedLight;
  }
}

void Preferences::setTheme(Expression::Theme theme) {
  if (theme != m_theme) {
    m_theme = theme;
  }
}

}
