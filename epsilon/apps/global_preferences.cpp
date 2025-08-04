#include "global_preferences.h"

#include "apps_container_helper.h"

GlobalPreferences GlobalPreferences::GlobalPreferencesInstance;
GlobalPreferences::GlobalPreferencesData* GlobalPreferences::s_data = nullptr;

void GlobalPreferences::Init() {
  Ion::Storage::FileSystem::sharedFileSystem
      ->initSystemRecord<GlobalPreferencesData>();
  s_data = Ion::Storage::FileSystem::sharedFileSystem
               ->findSystemRecord<GlobalPreferencesData>();
}

GlobalPreferences* GlobalPreferences::SharedGlobalPreferences() {
  assert(Ion::Storage::FileSystem::sharedFileSystem
             ->findSystemRecord<GlobalPreferencesData>() == s_data);
  return &GlobalPreferencesInstance;
}

void GlobalPreferences::setBrightnessLevel(int brightnessLevel) {
  if (s_data->m_brightnessLevel != brightnessLevel) {
    brightnessLevel = brightnessLevel < 0 ? 0 : brightnessLevel;
    brightnessLevel = brightnessLevel > Ion::Backlight::MaxBrightness
                          ? Ion::Backlight::MaxBrightness
                          : brightnessLevel;
    s_data->m_brightnessLevel = brightnessLevel;
    Ion::Backlight::setBrightness(s_data->m_brightnessLevel);
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
