#include "global_preferences.h"

#include "apps_container_helper.h"

static GlobalPreferences* fetchFromStorage() {
  Ion::Storage::Record record =
      Ion::Storage::FileSystem::sharedFileSystem->recordBaseNamedWithExtension(
          GlobalPreferences::k_recordName, Ion::Storage::systemExtension);
  assert(!record.isNull());
  Ion::Storage::Record::Data data = record.value();
  assert(data.size == sizeof(GlobalPreferences));
  return static_cast<GlobalPreferences*>(const_cast<void*>(data.buffer));
}

GlobalPreferences* GlobalPreferences::SharedGlobalPreferences() {
  static GlobalPreferences* ptr = fetchFromStorage();
  assert(fetchFromStorage() == ptr);
  return ptr;
}

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

/* m_brightnessLevel is stored in unaligned memory, and is sometimes converted
 * to floating-point. If brightnessLevel() was inlined, it could lead to
 * unaligned FPU access. */
int __attribute__((noinline)) GlobalPreferences::brightnessLevel() const {
  return m_brightnessLevel;
}
