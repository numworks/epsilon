#pragma once

#include "angle_unit.h"
#include "complex_format.h"
#include "preferences.h"
#include "symbol_context.h"

namespace Poincare {

// User settings needed by reduction and approximation algorithms
struct UserSettings {
#if POINCARE_COMPLEX
  ComplexFormat complexFormat = ComplexFormat::None;
#else
  constexpr static ComplexFormat complexFormat = ComplexFormat::Real;
#endif
  AngleUnit angleUnit = AngleUnit::None;
  const Poincare::SymbolContext& context = k_emptySymbolContext;
};

// Extended user settings (including unit setting)
struct UserSettingsWithUnit {
#if POINCARE_COMPLEX
  ComplexFormat complexFormat = ComplexFormat::None;
#else
  constexpr static ComplexFormat complexFormat = ComplexFormat::Real;
#endif
  AngleUnit angleUnit = AngleUnit::None;
  const Poincare::SymbolContext& context = k_emptySymbolContext;
  Preferences::UnitFormat unitFormat = Preferences::UnitFormat::Metric;

  // Implicit conversion to UserSettings
  operator UserSettings() const {
    return UserSettings {
#if POINCARE_COMPLEX
      .complexFormat = complexFormat,
#endif
      .angleUnit = angleUnit, .context = context
    };
  }
};

/* Conversion from UserSettings to UserSettingsWithUnit, UnitFormat will be
 * default */
inline UserSettingsWithUnit ToUserSettingsWithUnit(
    const UserSettings& userSettings) {
  return UserSettingsWithUnit {
#if POINCARE_COMPLEX
    .complexFormat = userSettings.complexFormat,
#endif
    .angleUnit = userSettings.angleUnit, .context = userSettings.context
  };
}

}  // namespace Poincare
