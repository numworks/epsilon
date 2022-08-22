#include "global_preferences.h"

GlobalPreferences * GlobalPreferences::sharedGlobalPreferences() {
  static GlobalPreferences globalPreferences;
  return &globalPreferences;
}

void GlobalPreferences::setCountry(I18n::Country country) {
  m_country = country;
  Poincare::Preferences::sharedPreferences()->setCombinatoricSymbols(combinatoricsSymbols());
  Poincare::Preferences::sharedPreferences()->enableMixedFractions(mixedFractions());
}

void GlobalPreferences::setBrightnessLevel(int brightnessLevel) {
  if (m_brightnessLevel != brightnessLevel) {
    brightnessLevel = brightnessLevel < 0 ? 0 : brightnessLevel;
    brightnessLevel = brightnessLevel > Ion::Backlight::MaxBrightness ? Ion::Backlight::MaxBrightness : brightnessLevel;
    m_brightnessLevel = brightnessLevel;
    Ion::Backlight::setBrightness(m_brightnessLevel);
  }
}

int GlobalPreferences::sequencesInitialRank() const {
  switch(preferences().sequencesInitialRank()) {
  case CountryPreferences::SequencesInitialRank::Zero:
    return 0;
  default:
    assert(preferences().sequencesInitialRank() == CountryPreferences::SequencesInitialRank::One);
    return 1;
  }
}
