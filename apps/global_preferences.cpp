#include "global_preferences.h"

#include "apps_container_helper.h"

GlobalPreferences* GlobalPreferences::SharedGlobalPreferences() {
  static GlobalPreferences* ptr = Ion::Storage::FileSystem::sharedFileSystem
                                      ->findSystemRecord<GlobalPreferences>();
  assert(Ion::Storage::FileSystem::sharedFileSystem
             ->findSystemRecord<GlobalPreferences>() == ptr);
  return ptr;
}

void GlobalPreferences::countryHasChanged(bool updateSnapshots) {
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
  switch (countryPreferences().sequencesInitialRank()) {
    case CountryPreferences::SequencesInitialRank::Zero:
      return 0;
    default:
      assert(countryPreferences().sequencesInitialRank() ==
             CountryPreferences::SequencesInitialRank::One);
      return 1;
  }
}
