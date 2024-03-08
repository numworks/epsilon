#include "global_preferences.h"

#include "apps_container_helper.h"

OMG::GlobalBox<GlobalPreferences> GlobalPreferences::sharedGlobalPreferences;

void GlobalPreferences::setCountry(I18n::Country country,
                                   bool updateSnapshots) {
  m_country = country;
  Poincare::Preferences::SharedPreferences()->setCombinatoricSymbols(
      combinatoricsSymbols());
  Poincare::Preferences::SharedPreferences()->enableMixedFractions(
      mixedFractions());
  Poincare::Preferences::SharedPreferences()->setLogarithmBasePosition(
      logarithmBasePosition());
  Poincare::Preferences::SharedPreferences()->setLogarithmKeyEvent(
      logarithmKeyEvent());
  Poincare::Preferences::SharedPreferences()->setParabolaParameter(
      parabolaParameter());
  if (!updateSnapshots) {
    return;
  }
  AppsContainerHelper::notifyCountryChangeToSnapshots();
}

void GlobalPreferences::setBrightnessLevel(int brightnessLevel) {
  if (m_brightnessLevel != brightnessLevel) {
    brightnessLevel = brightnessLevel < 0 ? 0 : brightnessLevel;
    brightnessLevel = brightnessLevel > Ion::Backlight::MaxBrightness
                          ? Ion::Backlight::MaxBrightness
                          : brightnessLevel;
    m_brightnessLevel = brightnessLevel;
    Ion::Backlight::setBrightness(m_brightnessLevel);
  }
}

int GlobalPreferences::sequencesInitialRank() const {
  switch (preferences().sequencesInitialRank()) {
    case CountryPreferences::SequencesInitialRank::Zero:
      return 0;
    default:
      assert(preferences().sequencesInitialRank() ==
             CountryPreferences::SequencesInitialRank::One);
      return 1;
  }
}
