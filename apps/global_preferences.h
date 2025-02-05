#ifndef APPS_GLOBAL_PREFERENCES_H
#define APPS_GLOBAL_PREFERENCES_H

#include <apps/i18n.h>
#include <ion.h>
#include <kandinsky/font.h>
#include <omg/global_box.h>
#include <poincare/preferences.h>

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

/* GlobalPreferences live in the Storage, which does not enforce alignment. The
 * packed attribute ensures the compiler will not emit instructions that require
 * the data to be aligned. */
class __attribute__((packed)) GlobalPreferences {
  friend OMG::GlobalBox<GlobalPreferences>;
  friend Ion::Storage::FileSystem;

  /* Friend class for test purposes that can access the GlobalPreferences
   * private constructor. */
  friend class GlobalPreferencesTestBuilder;

 public:
  constexpr static char k_recordName[] = "gp";

  constexpr static I18n::Country k_defaultCountry = I18n::Country::WW;

  static constexpr const CountryPreferences& defaultCountryPreferences() {
    return I18n::CountryPreferencesArray[static_cast<uint8_t>(
        k_defaultCountry)];
  }

  static GlobalPreferences* SharedGlobalPreferences();

  bool operator==(const GlobalPreferences&) const = default;

  I18n::Language language() const { return m_language; }
  void setLanguage(I18n::Language language) { m_language = language; }
  I18n::Country country() const { return m_country; }
  void setCountry(I18n::Country country, bool updateSnapshots = true) {
    m_country = country;
    countryHasChanged(updateSnapshots);
  }
  void countryHasChanged(bool updateSnapshots = true);

  constexpr CountryPreferences::AvailableExamModes availableExamModes() const {
    return countryPreferences().availableExamModes();
  }
  constexpr CountryPreferences::MethodForQuartiles methodForQuartiles() const {
    return countryPreferences().methodForQuartiles();
  }
  constexpr CountryPreferences::OutlierDefaultVisibility outliersStatus()
      const {
    return countryPreferences().outliersStatus();
  }
  constexpr CountryPreferences::HistogramsOffset histogramOffset() const {
    return countryPreferences().histogramOffset();
  }
  constexpr Poincare::Preferences::UnitFormat unitFormat() const {
    return countryPreferences().unitFormat();
  }
  constexpr CountryPreferences::HomeAppsLayout homeAppsLayout() const {
    return countryPreferences().homeAppsLayout();
  }
  constexpr const char* discriminantSymbol() const {
    return countryPreferences().discriminantSymbol();
  }
  constexpr const char* yPredictedSymbol() const {
    return countryPreferences().yPredictedSymbol();
  }
  constexpr CountryPreferences::StatsRowsLayout statsRowsLayout() const {
    return countryPreferences().statsRowsLayout();
  }
  constexpr Poincare::Preferences::CombinatoricSymbols combinatoricsSymbols()
      const {
    return countryPreferences().combinatoricSymbols();
  }
  constexpr CountryPreferences::ListsStatsOrderInToolbox
  listsStatsOrderInToolbox() const {
    return countryPreferences().listsStatsOrderInToolbox();
  }
  constexpr Poincare::Preferences::MixedFractions mixedFractions() const {
    return countryPreferences().mixedFractions();
  }
  constexpr CountryPreferences::RegressionApp regressionAppVariant() const {
    return countryPreferences().regressionAppVariant();
  }
  constexpr CountryPreferences::GraphTemplatesLayout graphTemplatesLayout()
      const {
    return countryPreferences().graphTemplatesLayout();
  }
  constexpr Poincare::Preferences::LogarithmBasePosition logarithmBasePosition()
      const {
    return countryPreferences().logarithmBasePosition();
  }
  constexpr Poincare::Preferences::LogarithmKeyEvent logarithmKeyEvent() const {
    return countryPreferences().logarithmKeyEvent();
  }
  constexpr Poincare::Preferences::ParabolaParameter parabolaParameter() const {
    return countryPreferences().parabolaParameter();
  }
  constexpr CountryPreferences::SolverDoubleRootName solverDoubleRootName()
      const {
    return countryPreferences().solverDoubleRootName();
  }
  constexpr CountryPreferences::GraphTemplateDefault graphTemplateDefault()
      const {
    return countryPreferences().graphTemplateDefault();
  }
  int sequencesInitialRank() const;

  const char* openIntervalChar(bool left) const {
    /* This should be done by country instead of language. However, some
     * messages have intervals. The toolbox would have to contain buffers
     * instead of messages to apply the correct interval. We decided that for
     * now that it's a language preference only. */
    return I18n::translate(left ? I18n::Message::OpenLeftInterval
                                : I18n::Message::OpenRightInterval);
  }

  bool showPopUp() const { return m_showPopUp; }
  void setShowPopUp(bool showPopUp) { m_showPopUp = showPopUp; }

  constexpr static int NumberOfBrightnessStates = 12;
  int brightnessLevel() const { return m_brightnessLevel; }
  void setBrightnessLevel(int brightnessLevel);

  int dimmingTime() const { return m_dimmingTime; }
  void setDimmingTime(int dimmingTime) { m_dimmingTime = dimmingTime; }

  KDFont::Size font() const { return m_font; }
  void setFont(KDFont::Size font) { m_font = font; }

  // In milliseconds
  constexpr static uint32_t k_defaultDimmingTime = 30 * 1000;

 private:
  constexpr static uint8_t k_version = 1;

  /* GlobalPreferences is a singleton, hence the private constructor. The unique
   * instance can be accessed through the
   * GlobalPreferences::SharedGlobalPreferences() pointer.
   */
  GlobalPreferences()
      : m_version(k_version),
        m_brightnessLevel(Ion::Backlight::MaxBrightness),
        m_language(k_defaultLanguage),
        m_country(k_defaultCountry),
        m_showPopUp(true),
        m_font(KDFont::Size::Large),
        m_dimmingTime(k_defaultDimmingTime) {}

  const CountryPreferences& countryPreferences() const {
    return I18n::CountryPreferencesArray[static_cast<uint8_t>(m_country)];
  }

  /* There should already have been an error when processing an empty
   * EPSILON_I18N flag */
  static_assert(I18n::NumberOfLanguages > 0,
                "I18n::NumberOfLanguages is not superior to 0");
  /* There should already have been an error when processing an empty
   * EPSILON_COUNTRIES flag */
  static_assert(I18n::NumberOfCountries > 0,
                "I18n::NumberOfCountries is not superior to 0");

#if __EMSCRIPTEN__
  using BrightnessType = emscripten_align1_int;
  using DimmingTimeType = emscripten_align1_int;
#else
  using BrightnessType = int;
  using DimmingTimeType = uint32_t;
#endif

  CODE_GUARD(global_preferences, 3087701149,    //
             uint8_t m_version;                 //
             BrightnessType m_brightnessLevel;  //
             I18n::Language m_language;         //
             I18n::Country m_country;           //
             bool m_showPopUp;                  //
             KDFont::Size m_font;               //
             DimmingTimeType m_dimmingTime;     // in milliseconds
             public
             : static constexpr int k_objectSize = 13;)
};

#if PLATFORM_DEVICE
static_assert(sizeof(GlobalPreferences) == GlobalPreferences::k_objectSize,
              "Class GlobalPreferences changed size");
#endif

#if __EMSCRIPTEN
/* GlobalPreferences live in the Storage which does not enforce alignment, so
 * make sure Emscripten cannot attempt unaligned accesses. */
static_assert(std::alignment_of<GlobalPreferences>() == 1);
#endif

#endif
